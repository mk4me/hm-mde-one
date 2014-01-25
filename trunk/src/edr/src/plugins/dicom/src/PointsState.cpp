#include "DicomPCH.h"
#include "PointsState.h"
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsSceneMouseEvent>
#include <QtGui/QGraphicsRectItem>
#include <QtGui/QGraphicsLineItem>
#include <QtGui/QMenu>
#include <coreui/AbstractStateMachine.h>
#include <coreui/MoveCommand.h>
#include <coreUi/MultiCommand.h>
#include "LayeredStateMachine.h"
#include "BackgroundLayer.h"
#include "LayeredSerie.h"
#include "PointsLayer.h"
#include "Adnotations.h"


using namespace dicom;


class ClearCommand : public utils::ICommand
{
public:
    ClearCommand(PointsLayerPtr layer) : 
      layer(layer)
    {
    }

    virtual void doIt()
    {
        layer->getItem()->setVisible(false);
    }

    virtual void undoIt()
    {
        layer->getItem()->setVisible(true);
    }

    virtual QString name()
    {
        return QString(typeid(this).name());
    }

private:
    PointsLayerPtr layer;
};

namespace dicom {
class AddLayerCommand : public utils::ICommand
{
public:
    AddLayerCommand(LayeredStateMachine* machine, ILayeredImagePtr image, PointsState* pointsState, PointsLayerPtr layer, int adnotationIdx) : 
      layer(layer),
      adnotationIdx(adnotationIdx),
      image(image),
      machine(machine),
      pointsState(pointsState)
    {
    }

public:
    virtual void doIt()
    {
        layer->setEditable(false);
        layer->setAdnotationIdx(adnotationIdx);
        image->addLayer(layer, machine->getSerie()->getUserName());
        machine->getSerie()->refresh();
        machine->getSerie()->save();
    }
    
    virtual void undoIt() {
        image->removeLayer(layer);
        /*if (pointsState == machine->getCurveState().get()) {
            machine->setState(machine->getCurveState());
        } else {
            machine->setState(machine->getPolyState());
        }
*/
        machine->getSerie()->refresh();
        machine->getSerie()->save();
        layer->setEditable(true);
        pointsState->layer = layer;
    }
    
    virtual QString name() { return QString(typeid(this).name()); }

private:
    PointsLayerPtr layer;
    int adnotationIdx;
    ILayeredImagePtr image;
    LayeredStateMachine* machine;
    PointsState* pointsState;
};


}


dicom::PointsState::PointsState( LayeredStateMachine* machine, bool curved, bool openLine, adnotations::annotationsIdx adnotationIdx ) :
    coreUI::AbstractState(machine),
    machine(machine),
    possibleMove(false),
    curved(curved),
    openLine(openLine),
    adnotationIdx(adnotationIdx)
{

}


bool dicom::PointsState::keyReleaseEvent( QKeyEvent *event )
{
    return true;
}


bool dicom::PointsState::mousePressEvent( QGraphicsSceneMouseEvent* e )
{
    if (e->button() == Qt::RightButton && layer->getNumPoint() > 0) {
        QMenu menu;
        QAction* clearAction = menu.addAction(tr("Remove"));
        
        connect(clearAction, SIGNAL(triggered()), this, SLOT(clear()));

        QAction* addAction = menu.addAction(tr("Save"));
        connect(addAction, SIGNAL(triggered()), this, SLOT(addLayer()));
        menu.exec(e->screenPos());
    } else if (e->button() == Qt::LeftButton) {
        auto item = machine->getGraphicsScene()->itemAt(e->scenePos());
        if (layer->hasPoint(item)) {
            possibleMove = true;
            positionsToCheck.clear();
            int count = layer->getNumPoint();
            for (int i = 0; i < count; ++i) {
                auto pointItem = layer->getPoint(i);
                positionsToCheck.push_back(std::make_pair(pointItem, pointItem->scenePos()));
            }
            return false;
        } else {
            auto pos = e->scenePos();
            machine->getCommandStack()->addCommand(utils::make_shared<AddPointCommand>(layer, pos));
        }
    }

    return true;
}


void dicom::PointsState::clear()
{
    machine->getCommandStack()->addCommand(utils::make_shared<ClearCommand>(layer));
    machine->setState(machine->getNormalState());
}

void dicom::PointsState::addLayer()
{
    addLayer(adnotationIdx);
    resetLayer();
    machine->setState(machine->getNormalState());
}

void dicom::PointsState::addLayer( int adnIdx )
{
    // warunek isVisible wynika z tego, ze clear nie usuwa punktow tylko robi warstwe niewidoczna
    if (layer->getNumPoint() > 0 && layer->getItem()->isVisible()) {
        auto img = machine->getSerie()->getImage();

        auto command = utils::make_shared<AddLayerCommand>(machine, img, this, layer, adnIdx);
        machine->getCommandStack()->addCommand(command);
    }
}

void dicom::PointsState::begin( coreUI::AbstractStateConstPtr lastState )
{
    if (this->curved) {
        QCursor c(QPixmap(":/dicom/add_curve.png"), 0, 0);
        machine->changeCursor(c);
    } else {
        QCursor c(QPixmap(":/dicom/add_poly.png"), 0, 0);
        machine->changeCursor(c);
    }
    resetLayer();
    ILayeredImagePtr img = machine->getSerie()->getImage();
    BOOST_FOREACH(std::string tag, img->getTags()) {
        for (int i = img->getNumLayerItems(tag) - 1; i >= 0; --i) {
            img->getLayerItem(tag, i)->setSelected(false);
        }
    }
    machine->getGraphicsScene()->addItem(layer->getItem());
}

void dicom::PointsState::end()
{
    if (layer->getNumPoint() > 0) {
        addLayer(adnotationIdx);
        resetLayer();
    }
    machine->changeCursor(Qt::ArrowCursor);
}

bool dicom::PointsState::mouseReleaseEvent( QGraphicsSceneMouseEvent* e )
{
    possibleMove = false;

    std::vector<utils::ICommandPtr> commands;
    for (auto it = positionsToCheck.begin(); it != positionsToCheck.end(); ++it) {
        QGraphicsItem* item = it->first;
        QPointF oldP = it->second;
        QPointF newP = item->pos();
        if (newP != oldP) {
            commands.push_back(utils::ICommandPtr(new MovePathCommand(layer, item, newP, oldP)));
        }
    }
    if (!commands.empty()) {
        machine->getCommandStack()->addCommand(utils::ICommandPtr(new coreUI::MultiCommand(commands)));
    }
    return false;
}

bool dicom::PointsState::mouseMoveEvent( QGraphicsSceneMouseEvent* e )
{
    if (possibleMove) {
        layer->refresh();
    }
    return false;
}

void dicom::PointsState::resetLayer()
{
    layer = utils::make_shared<PointsLayer>();
    layer->setPointsDrawer(DrawersBuilder::createDrawer(adnotationIdx));
}

