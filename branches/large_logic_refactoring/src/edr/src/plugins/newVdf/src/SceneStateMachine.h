/********************************************************************
	created:	2012/12/20
	created:	20:12:2012   11:15
	filename: 	SceneStateMachine.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__SCENESTATEMACHINE_H__
#define HEADER_GUARD_NEWVDF__SCENESTATEMACHINE_H__

#include <QtGui/QWidget>
#include <QtGui/QGraphicsView>
#include <QtGui/QWheelEvent>

#include "ISceneState.h"
#include "NormalState.h"
#include "GroupState.h"
#include "ConnectState.h"
#include "Command.h"

namespace vdf {

class VdfScene;
class NewVdfWidget;

class SceneStateMachine : public QObject
{
    Q_OBJECT;
public:
    SceneStateMachine(NewVdfWidget* widget);
    virtual ~SceneStateMachine() {}

public:
    NormalStatePtr getNormalState();
    ConnectStatePtr getConnectState();
	PreGroupStatePtr getPreGroupState();
	GroupStatePtr getGroupState();
	GroupSelectedStatePtr getGroupSelectedState();

    void setState(ISceneStatePtr state);
	VdfScene* getScene() const;
	SceneModelPtr getSceneModel() const;
	CommandStackPtr getCommandStack() const;

public:
    bool mousePressEvent(QGraphicsSceneMouseEvent* e);
    bool mouseReleaseEvent(QGraphicsSceneMouseEvent* e);
    bool mouseDoubleClickEvent(QGraphicsSceneMouseEvent* e);
    bool mouseMoveEvent(QGraphicsSceneMouseEvent* e);
	bool keyPressEvent(QKeyEvent *event);
	bool keyReleaseEvent(QKeyEvent *event);

#ifndef QT_NO_WHEELEVENT
    bool wheelEvent(QGraphicsSceneWheelEvent* e);
#endif

public slots:
    void selectionChanged();

private:
    NormalStatePtr normalState;
    ConnectStatePtr connectState;
    ISceneStatePtr currentState;
	PreGroupStatePtr preGroupState;
	GroupStatePtr groupState;
	GroupSelectedStatePtr groupSelectedState;
	NewVdfWidget* vdfWidget;
};
typedef core::shared_ptr<SceneStateMachine> SceneStateMachinePtr;
typedef core::shared_ptr<const SceneStateMachine> SceneStateMachineConstPtr;

}

#endif
