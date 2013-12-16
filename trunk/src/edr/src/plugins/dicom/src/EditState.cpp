#include "DicomPCH.h"
#include "EditState.h"
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


dicom::EditState::EditState( LayeredStateMachine* machine ) :
    coreUI::AbstractState(machine),
    machine(machine),
    possibleMove(false)
{

}


bool dicom::EditState::keyReleaseEvent( QKeyEvent *event )
{
    return true;
}

bool dicom::EditState::mousePressEvent( QGraphicsSceneMouseEvent* e )
{
    if (e->button() == Qt::RightButton) {
        QMenu menu;
        QAction* doneAction = menu.addAction(tr("Done"));
        connect(doneAction, SIGNAL(triggered()), this, SLOT(done()));
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
            auto offs = layer->getItem()->pos();
            machine->getCommandStack()->addCommand(utils::make_shared<AddPointCommand>(layer, pos - offs));
        }
    }

    return true;
}

void dicom::EditState::begin( coreUI::AbstractStateConstPtr lastState )
{
    layer = getLayerToEdit();
    layer->setEditable(true);
}

void dicom::EditState::end()
{
    //done();
    layer->setEditable(false);
}

bool dicom::EditState::mouseReleaseEvent( QGraphicsSceneMouseEvent* e )
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

bool dicom::EditState::mouseMoveEvent( QGraphicsSceneMouseEvent* e )
{
    if (possibleMove) {
        layer->refresh();
    }
    return false;
}

void dicom::EditState::done()
{
    machine->setState(machine->getNormalState());
}

QString dicom::AddPointCommand::name()
{
    return QString(typeid(this).name());
}

void dicom::AddPointCommand::undoIt()
{
    auto point = layer->removePoint(idx);
    removedPoint = std::unique_ptr<QGraphicsItem>(point);
    removedPoint->setVisible(false);
    layer->refresh();
}

void dicom::AddPointCommand::doIt()
{
    if (removedPoint) {
        removedPoint->setVisible(true);
        layer->addPoint(removedPoint.release());
        removedPoint = std::unique_ptr<QGraphicsItem>();
    } else {
        idx = layer->getNumPoint();
        layer->addPoint(newP);
    }
}

dicom::AddPointCommand::AddPointCommand( PointsLayerPtr layer, const QPointF& newP ) : 
layer(layer),
    newP(newP),
    idx(-1),
    removedPoint(nullptr)
{

}

void dicom::MovePathCommand::undoIt()
{
    MoveCommand::undoIt();
    layer->refresh();
}

void dicom::MovePathCommand::doIt()
{
    MoveCommand::doIt();
    layer->refresh();
}

dicom::MovePathCommand::MovePathCommand( PointsLayerPtr layer, QGraphicsItem* item, const QPointF& newP, const QPointF& oldP ) : 
MoveCommand(item, newP, oldP),
    layer(layer)
{

}
