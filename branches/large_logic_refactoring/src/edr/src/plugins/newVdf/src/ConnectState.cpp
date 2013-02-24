#include "NewVdfPCH.h"
#include "ConnectState.h"
#include "SimpleItem.h"
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsView>
#include <QtGui/QHBoxLayout>
#include "SceneStateMachine.h"
#include "VdfScene.h"

using namespace vdf;

void ConnectState::begin( ISceneStateConstPtr lastState )
{
    auto normalState = stateMachine->getNormalState();
    UTILS_ASSERT (normalState == lastState);
    
    firstPin = normalState->getSelectedPin();
	auto sceneModel = stateMachine->getScene()->getSceneModel();
	connections = sceneModel->getPossibleConections(firstPin);

	for (auto it = connections.possible.begin(); it != connections.possible.end(); ++it) {
		(*it)->markConnective();
	}
	for (auto it = connections.impossible.begin(); it != connections.impossible.end(); ++it) {
		(*it)->markUnconnective();
	}
    auto pos = firstPin->visualItem()->scenePos();
    auto factories = stateMachine->getSceneModel()->getBuilder().getFactories();
    connection = factories->getCurrentConnectionsFactory()->createConnection();
    pin1 = factories->getCurrentPinsFactory()->createInputPin();
    pin2 = factories->getCurrentPinsFactory()->createOutputPin();
    connection->setBegin(pin1);
    connection->setEnd(pin2);
    stateMachine->getScene()->addItem(connection->visualItem());
    pin1->visualItem()->setPos(pos);
    pin2->visualItem()->setPos(pos);

    //tempLine = stateMachine->getScene()->addLine(pos.x(), pos.y(), pos.x(), pos.y());
    //tempLine->setPen(QPen(Qt::red));
}

ConnectState::ConnectState( SceneStateMachine* scene ) : 
    ISceneState(scene)
{

}

void ConnectState::selectionChanged( const QList<QGraphicsItem*>& list )
{
    //if (list.size() == 1) {
    //    auto* item = list.first();
	//	auto model = stateMachine->getScene()->getSceneModel();
	//	IVisualPinPtr pin1 = firstPin;
	//	IVisualPinPtr pin2 = core::dynamic_pointer_cast<IVisualPin>(model->tryGetVisualItem(item));
    //    if (pin1 && pin2 && model->connectionPossible(pin1, pin2)) {
	//		stateMachine->getCommandStack()->addCommand(ICommandPtr(new AddConnectionCommand(model, pin1, pin2)));
    //        //model->addConnection(pin1, pin2);
    //        stateMachine->setState(stateMachine->getNormalState());
    //    }
    //}
}

bool ConnectState::mouseMoveEvent( QGraphicsSceneMouseEvent* e )
{
    //UTILS_ASSERT(tempLine);

    auto pos1 = firstPin->visualItem()->scenePos();
    auto pos2 = e->scenePos();
    //tempLine->setLine(pos1.x(), pos1.y(), pos2.x(), pos2.y());
    //tempLine->update();
    pin2->visualItem()->setPos(e->scenePos());
    connection->visualItem()->update();
    return true;
}

void ConnectState::end()
{
    //delete tempLine;
    //tempLine = nullptr;
    stateMachine->getScene()->removeItem(connection->visualItem());
    connection = IVisualConnectionPtr();
    pin1 = IVisualPinPtr();
    pin2 = IVisualPinPtr();
	for (auto it = connections.possible.begin(); it != connections.possible.end(); ++it) {
		(*it)->markNormal();
	}
	for (auto it = connections.impossible.begin(); it != connections.impossible.end(); ++it) {
		(*it)->markNormal();
	}
}

bool ConnectState::mousePressEvent( QGraphicsSceneMouseEvent* e )
{
	if (e->button() == Qt::RightButton) {
		stateMachine->setState(stateMachine->getNormalState());
		return true;
	} else if (e->button() == Qt::LeftButton) {
        auto model = stateMachine->getScene()->getSceneModel();
        IVisualPinPtr pin1 = firstPin;

        auto items = stateMachine->getScene()->items(e->scenePos());
        for (auto it = items.begin(); it != items.end(); ++it) {
            IVisualPinPtr pin2 = core::dynamic_pointer_cast<IVisualPin>(model->tryGetVisualItem(*it));
            if (pin1 && pin2 && model->connectionPossible(pin1, pin2)) {
                stateMachine->getCommandStack()->addCommand(ICommandPtr(new AddConnectionCommand(model, pin1, pin2)));
                //model->addConnection(pin1, pin2);
                break;
            }
        }

        stateMachine->setState(stateMachine->getNormalState());
        return true;
    }

	// tworzenie polaczenia - w selectionChanged...
	return false;
}
