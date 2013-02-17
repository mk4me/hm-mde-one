#include "NewVdfPCH.h"
#include "GroupState.h"
#include "SceneStateMachine.h"
#include "VdfScene.h"


void PreGroupState::selectionChanged( const QList<QGraphicsItem*>& list )
{
	stateMachine->setState(stateMachine->getNormalState());
}

bool PreGroupState::mousePressEvent( QGraphicsSceneMouseEvent* e )
{
	if (e->button() == Qt::LeftButton) {
		startPos = e->scenePos();
		stateMachine->setState(stateMachine->getGroupState());
	} else {
		stateMachine->setState(stateMachine->getNormalState());
	}
	return false;
}

void PreGroupState::begin( ISceneStateConstPtr lastState )
{
	stateMachine->getScene()->changeCursor(Qt::CrossCursor);
}

void PreGroupState::end()
{
	stateMachine->getScene()->changeCursor(Qt::ArrowCursor);
}

void GroupState::begin( ISceneStateConstPtr lastState )
{
	auto preGroup = stateMachine->getPreGroupState();
	UTILS_ASSERT(preGroup == lastState);
	stateMachine->getScene()->changeCursor(Qt::CrossCursor);
	startPos = preGroup->getStartPos();
	tempRect = stateMachine->getScene()->addRect(startPos.x(), startPos.y(), 1, 1);
}

void GroupState::end()
{
	stateMachine->getScene()->changeCursor(Qt::ArrowCursor);
	delete tempRect;
	tempRect = nullptr;
}

bool GroupState::mouseMoveEvent( QGraphicsSceneMouseEvent* e )
{
	tempRect->setRect(getRect(startPos, e->scenePos()));
	return true;
}

bool GroupState::mousePressEvent( QGraphicsSceneMouseEvent* e )
{
	if (e->button() == Qt::LeftButton) {
		QRectF rect = getRect(startPos, e->scenePos());
		auto selected = stateMachine->getScene()->items(rect, Qt::ContainsItemBoundingRect, Qt::AscendingOrder);
		selectedNodes = stateMachine->getScene()->getSceneModel()->getVisualItems<IVisualNodePtr>(selected);
		if (selectedNodes.size() > 0) {
			for (auto it = selectedNodes.begin(); it != selectedNodes.end(); ++it) {
				(*it)->addSelection();
			}
			//stateMachine->setState(stateMachine->getGroupSelectedState());
			stateMachine->setState(stateMachine->getNormalState());
			return true;
		}
	} 
	
	stateMachine->setState(stateMachine->getNormalState());
	return true;
}

GroupState::GroupState( SceneStateMachine* scene ) :
	ISceneState(scene),
	tempRect(nullptr)
{

}

QRectF GroupState::getRect( const QPointF& startPos, const QPointF& endPos )
{
	auto x1 = startPos.x();
	auto y1 = startPos.y();
	auto x2 = endPos.x();
	auto y2 = endPos.y();
	auto x = (std::min)(x1, x2);
	auto y = (std::min)(y1, y2);
	auto w = (std::abs)(x1 - x2);
	auto h = (std::abs)(y1 - y2);
	return QRectF(x, y, w, h);
}

GroupSelectedState::GroupSelectedState( SceneStateMachine* scene ) :
	ISceneState(scene)
{

}

void GroupSelectedState::begin( ISceneStateConstPtr lastState )
{
	auto groupState = stateMachine->getGroupState();
	UTILS_ASSERT(groupState == lastState);
	selectedNodes = groupState->getSelectedNodes();
	for (auto it = selectedNodes.begin(); it != selectedNodes.end(); ++it) {
		(*it)->addSelection();
	}

}

void GroupSelectedState::end()
{

}

bool GroupSelectedState::mouseMoveEvent( QGraphicsSceneMouseEvent* e )
{
	return false;
}

bool GroupSelectedState::mousePressEvent( QGraphicsSceneMouseEvent* e )
{
	return false;
}
