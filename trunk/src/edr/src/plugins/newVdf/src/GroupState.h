/********************************************************************
	created:	2013/01/23
	created:	23:1:2013   9:13
	filename: 	GroupState.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__GROUPSTATE_H__
#define HEADER_GUARD_NEWVDF__GROUPSTATE_H__


#include "ISceneState.h"

namespace vdf {

class GroupState : public ISceneState
{
public:
	GroupState(SceneStateMachine* scene);
	virtual ~GroupState() {}

public:
	virtual void begin( ISceneStateConstPtr lastState );
	virtual void end();
	virtual bool mouseMoveEvent(QGraphicsSceneMouseEvent* e);
	virtual bool mousePressEvent(QGraphicsSceneMouseEvent* e);
	virtual bool mouseReleaseEvent(QGraphicsSceneMouseEvent* e);
	const QList<IVisualNodePtr>& getSelectedNodes() const { return selectedNodes; }

private:
	QRectF getRect(const QPointF& startPos, const QPointF&  endPos );

private:
	typedef core::shared_ptr<QPointF> PointPtr;
	QGraphicsRectItem* tempRect;
	PointPtr startPos;
	QList<IVisualNodePtr> selectedNodes;
};
typedef core::shared_ptr<GroupState> GroupStatePtr;
typedef core::shared_ptr<const GroupState> GroupStateConstPtr;

class GroupSelectedState : public ISceneState
{
public:
	GroupSelectedState(SceneStateMachine* scene);
	virtual ~GroupSelectedState() {}

public:
	virtual void begin( ISceneStateConstPtr lastState );
	virtual void end();
	virtual bool mouseMoveEvent(QGraphicsSceneMouseEvent* e);
	virtual bool mousePressEvent(QGraphicsSceneMouseEvent* e);

private:
	QList<IVisualNodePtr> selectedNodes;
};
typedef core::shared_ptr<GroupSelectedState> GroupSelectedStatePtr;
typedef core::shared_ptr<const GroupSelectedState> GroupSelectedStateConstPtr;

}

#endif
