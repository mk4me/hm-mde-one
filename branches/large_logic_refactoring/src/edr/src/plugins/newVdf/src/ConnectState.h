/********************************************************************
	created:	2013/01/23
	created:	23:1:2013   11:17
	filename: 	ConnectState.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__CONNECTSTATE_H__
#define HEADER_GUARD_NEWVDF__CONNECTSTATE_H__

#include "ISceneState.h"

//!
class ConnectState : public ISceneState
{
public:
    ConnectState(SceneStateMachine* scene);
    virtual ~ConnectState() {}

public:
    virtual void begin( ISceneStateConstPtr lastState );
    virtual void end();
    virtual void selectionChanged(const QList<QGraphicsItem*>& list);
    virtual bool mouseMoveEvent(QGraphicsSceneMouseEvent* e);
	virtual bool mousePressEvent(QGraphicsSceneMouseEvent* e);

private:
    IVisualPinPtr firstPin;
    QGraphicsLineItem* tempLine;
	SceneModel::Connections connections;
};
typedef core::shared_ptr<ConnectState> ConnectStatePtr;
typedef core::shared_ptr<const ConnectState> ConnectStateConstPtr;


#endif
