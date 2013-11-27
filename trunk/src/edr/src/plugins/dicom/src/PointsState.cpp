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

//! Polecenie dodaje punkt do obszaru
class AddPointCommand : public utils::ICommand
{
public:
    //! Konstruktor
    //! \param item Element graficzny sceny, które zostanie przesuniêty 
    //! \param newP Nowa pozycja elementu 
    //! \param oldP Stara pozycja elementu
    AddPointCommand(PointsLayerPtr layer, const QPointF& newP) : 
       layer(layer),
       newP(newP),
       idx(-1),
       removedPoint(nullptr)
    {

    }

public:
    //! Przesuwa element
    virtual void doIt()
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

    //! Cofa przesuniêcie
    virtual void undoIt() {
        auto point = layer->removePoint(idx);
        removedPoint = std::unique_ptr<QGraphicsItem>(point);
        removedPoint->setVisible(false);
        layer->refresh();
    }
    //! Nazwa polecenia 
    virtual QString name() { return QString(typeid(this).name()); }

private:
    //! Modyfikowany obszar
    PointsLayerPtr layer;
    //! Nowododany punkt
    const QPointF& newP;
    //! indeks punktu do usuniêcia
    int idx;
    /// usuniêty punkt, który mo¿e zostaæ przywrócony. Przywracaj¹c nie mo¿na stworzyæ nowej instancji
    /// gdy¿ mog³oby to spowodowaæ b³êdy np. przy MoveCommand
    std::unique_ptr<QGraphicsItem> removedPoint;
};


class MovePathCommand : public coreUI::MoveCommand 
{
public:
    MovePathCommand(PointsLayerPtr layer, QGraphicsItem* item, const QPointF& newP, const QPointF& oldP) : 
      MoveCommand(item, newP, oldP),
      layer(layer)
    {
    }

    virtual void doIt()
    {
        MoveCommand::doIt();
        layer->refresh();
    }

    virtual void undoIt()
    {
        MoveCommand::undoIt();
        layer->refresh();
    }

private:
    PointsLayerPtr layer;
};

class Connection : public QGraphicsLineItem 
{
public:
    Connection(QGraphicsItem* A, QGraphicsItem* B) : 
      A(A), B(B)
    {
        setZValue(2.0);
        refresh();
    }

    void refresh() {
        auto pA = A->scenePos();
        auto pB = B->scenePos();
        setLine(pA.x(), pA.y(), pB.x(), pB.y());
    }

    virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0 )
    {
        refresh();
        QGraphicsLineItem::paint(painter, option, widget);
    }

private:
    QGraphicsItem* A;
    QGraphicsItem* B;
};


dicom::PointsState::PointsState( LayeredStateMachine* machine, bool curved ) :
    coreUI::AbstractState(machine),
    machine(machine),
    possibleMove(false),
    curved(curved)
{

}


bool dicom::PointsState::keyReleaseEvent( QKeyEvent *event )
{
    return true;
}


//bool dicom::PointsState::mouseReleaseEvent( QGraphicsSceneMouseEvent* e )
bool dicom::PointsState::mousePressEvent( QGraphicsSceneMouseEvent* e )
{
    if (e->button() == Qt::RightButton) {
        QMenu menu;
        QAction* clearAction = menu.addAction(tr("Clear"));
        //QAction* addLayerAction = menu.addAction(tr("Add layer"));

        QMenu* sub = menu.addMenu(QIcon(), tr("Add as:"));
        adnotations::AdnotationsTypePtr adn = adnotations::instance();
        for (auto it = adn->right.begin(); it != adn->right.end(); ++it) {
            QAction* a = sub->addAction(it->first);
            connect(a, SIGNAL(triggered()), this, SLOT(addLayer()));
        }

        connect(clearAction, SIGNAL(triggered()), this, SLOT(clear()));
        //connect(addLayerAction, SIGNAL(triggered()), this, SLOT(addLayer()));
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
            //layer->addPoint(pos);
            machine->getCommandStack()->addCommand(utils::make_shared<AddPointCommand>(layer, pos));
        }
    }

    return true;
}


void dicom::PointsState::clear()
{
    /*for (auto it = connections.begin(); it != connections.end(); ++it) {
        machine->getGraphicsScene()->removeItem(*it);
        delete *it;
    }
    connections.clear();

    for (auto it = points.begin(); it != points.end(); ++it) {
        machine->getGraphicsScene()->removeItem(*it);
        delete *it;
    }
    points.clear();*/

    /*if (layer) {
        machine->getGraphicsScene()->removeItem(layer->getItem());
    }*/
    resetLayer();

}

void dicom::PointsState::addLayer()
{
    /*PointsLayerPtr layer = utils::make_shared<PointsLayer>();
    for (auto it = points.begin(); it != points.end(); ++it) {
        layer->addPoint((*it)->scenePos().toPoint());
    }*/

    QAction* a = qobject_cast<QAction*>(sender());
    int adnIdx = adnotations::instance()->right.at(a->text());
    auto img = machine->getSerie()->getImage();
    layer->setEditable(false);
    layer->setAdnotationIdx(adnIdx);
    img->addLayer(layer);
    machine->getSerie()->refresh();
    machine->getSerie()->save();
    clear();

    machine->setState(machine->getNormalState());
}

void dicom::PointsState::begin( coreUI::AbstractStateConstPtr lastState )
{
    resetLayer();
    machine->getGraphicsScene()->addItem(layer->getItem());
}

void dicom::PointsState::end()
{
    clear();
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
    // TODO pozbyc sie tej flagi
    if (curved) {
        layer->setPointsDrawer(utils::make_shared<CurveDrawer>());
    } else {
        layer->setPointsDrawer(utils::make_shared<PolyDrawer>());
    }
}

