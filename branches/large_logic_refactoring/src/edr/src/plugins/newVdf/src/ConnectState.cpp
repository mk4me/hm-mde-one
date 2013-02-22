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
    tempLine = stateMachine->getScene()->addLine(pos.x(), pos.y(), pos.x(), pos.y());
    tempLine->setPen(QPen(Qt::red));
}

ConnectState::ConnectState( SceneStateMachine* scene ) : 
    ISceneState(scene)
{

}

void ConnectState::selectionChanged( const QList<QGraphicsItem*>& list )
{
    if (list.size() == 1) {
        auto* item = list.first();
		auto model = stateMachine->getScene()->getSceneModel();
		IVisualPinPtr pin1 = firstPin;
		IVisualPinPtr pin2 = core::dynamic_pointer_cast<IVisualPin>(model->tryGetVisualItem(item));
        if (pin1 && pin2 && model->connectionPossible(pin1, pin2)) {
			stateMachine->getCommandStack()->addCommand(ICommandPtr(new AddConnectionCommand(model, pin1, pin2)));
            //model->addConnection(pin1, pin2);
            stateMachine->setState(stateMachine->getNormalState());
        }
    }
}

bool ConnectState::mouseMoveEvent( QGraphicsSceneMouseEvent* e )
{
    UTILS_ASSERT(tempLine);

    auto pos1 = firstPin->visualItem()->scenePos();
    auto pos2 = e->scenePos();
    tempLine->setLine(pos1.x(), pos1.y(), pos2.x(), pos2.y());
    tempLine->update();
    return true;
}

void ConnectState::end()
{
    delete tempLine;
    tempLine = nullptr;
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
	} 

	// tworzenie polaczenia - w selectionChanged...
	return false;
}
