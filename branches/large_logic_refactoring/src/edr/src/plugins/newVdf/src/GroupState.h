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

class PreGroupState : public ISceneState
{
public:
	PreGroupState(SceneStateMachine* scene)  : ISceneState(scene) {}
	virtual ~PreGroupState() {}

public:
	virtual void begin( ISceneStateConstPtr lastState );
	virtual void end();
	virtual bool mousePressEvent(QGraphicsSceneMouseEvent* e);
	virtual void selectionChanged(const QList<QGraphicsItem*>& list);

	const QPointF& getStartPos() const { return startPos; }
private:
	QPointF startPos;
};
typedef core::shared_ptr<PreGroupState> PreGroupStatePtr;
typedef core::shared_ptr<const PreGroupState> PreGroupStateConstPtr;

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
	const QList<IVisualNodePtr>& getSelectedNodes() const { return selectedNodes; }

private:
	QRectF getRect(const QPointF& startPos, const QPointF&  endPos );

private:
	QGraphicsRectItem* tempRect;
	QPointF startPos;
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

#endif
