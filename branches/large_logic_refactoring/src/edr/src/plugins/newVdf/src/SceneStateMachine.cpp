#include "NewVdfPCH.h"
#include "SceneStateMachine.h"
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsView>
#include <QtGui/QHBoxLayout>
#include "SimpleItem.h"
#include "VdfScene.h"
#include "SceneModel.h"
#include "NewVdfWidget.h"

using namespace vdf;

SceneStateMachine::SceneStateMachine(NewVdfWidget* widget) : 
	vdfWidget(widget)
 {
    normalState = NormalStatePtr(new NormalState(this));
    connectState = ConnectStatePtr(new ConnectState(this));
	groupState = GroupStatePtr(new GroupState(this));
	groupSelectedState = GroupSelectedStatePtr(new GroupSelectedState(this));
    currentState = normalState;
 }

NormalStatePtr SceneStateMachine::getNormalState()
{
    return normalState; 
}

ConnectStatePtr SceneStateMachine::getConnectState() 
{
    return connectState; 
}

void SceneStateMachine::setState( ISceneStatePtr state )
{
    UTILS_ASSERT(currentState);
    currentState->end();
    state->begin(currentState);
    currentState = state;
}

bool SceneStateMachine::mousePressEvent( QGraphicsSceneMouseEvent* e )
{
    return currentState->mousePressEvent(e);
}

bool SceneStateMachine::mouseReleaseEvent( QGraphicsSceneMouseEvent* e )
{
    return currentState->mouseReleaseEvent(e);
}

bool SceneStateMachine::mouseDoubleClickEvent( QGraphicsSceneMouseEvent* e )
{
    return currentState->mouseDoubleClickEvent(e);
}

bool SceneStateMachine::mouseMoveEvent( QGraphicsSceneMouseEvent* e )
{
    return currentState->mouseMoveEvent(e);
}

bool SceneStateMachine::keyPressEvent(QKeyEvent *event)
{
	return currentState->keyPressEvent(event);
}

bool SceneStateMachine::keyReleaseEvent(QKeyEvent *event)
{
	return currentState->keyReleaseEvent(event);
}

#ifndef QT_NO_WHEELEVENT
bool SceneStateMachine::wheelEvent( QGraphicsSceneWheelEvent* e )
{
    return currentState->wheelEvent(e);
}
#endif

VdfScene* SceneStateMachine::getScene() const
{
     return vdfWidget->getScene(); 
}

void SceneStateMachine::selectionChanged()
{
    auto items = getScene()->selectedItems();
    currentState->selectionChanged(items);
    if (items.size() == 1) {
        auto node = getSceneModel()->tryGetVisualItem(*items.begin());
        if (node && node->isType(IVisualItem::Node)) {
            emit singleNodeSelected(core::dynamic_pointer_cast<IVisualNode>(node));
        }
    }
}


GroupStatePtr SceneStateMachine::getGroupState()
{
	return groupState;
}

GroupSelectedStatePtr SceneStateMachine::getGroupSelectedState()
{
	return groupSelectedState;
}

CommandStackPtr SceneStateMachine::getCommandStack() const
{
	return vdfWidget->getCommandStack();
}

SceneModelPtr SceneStateMachine::getSceneModel() const
{
	UTILS_ASSERT(getScene());
	return getScene()->getSceneModel();
}
