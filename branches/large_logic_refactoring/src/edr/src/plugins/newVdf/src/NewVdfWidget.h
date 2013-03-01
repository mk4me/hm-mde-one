/********************************************************************
	created:	2012/12/13
	created:	13:12:2012   12:10
	filename: 	NewVdfWidget.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__NEWVDFWIDGET_H__
#define HEADER_GUARD_NEWVDF__NEWVDFWIDGET_H__

#include <QtGui/QWidget>
#include <QtGui/QGraphicsView>
#include <QtGui/QWheelEvent>

#include "SceneModel.h"
#include "Command.h"

class QGraphicsScene;
class QMouseEvent;


namespace vdf {

class SimplePinItem;
class SceneStateMachine;
class VdfScene;  
class VdfView;

class NewVdfWidget : public QWidget
{
    Q_OBJECT;

public:
    NewVdfWidget(CommandStackPtr stack, SceneModelPtr sceneModel);
    virtual ~NewVdfWidget();

public:
    SceneModelPtr getSceneModel() const { return sceneModel; }
	VdfScene* getScene() const { return scene; }  
	VdfView* getView() const { return view; }
	CommandStackPtr getCommandStack() const { return commandStack; }

public slots:
	void undo();
	void redo();
	void merge();
	void deleteSelected();
	void clearScene();

signals:
    void singleNodeSelected(IVisualNodePtr node);

private:
    VdfScene* scene;  
    VdfView* view;
    SceneModelPtr sceneModel;
    bool connectionMode;
	CommandStackPtr commandStack;
	core::shared_ptr<SceneStateMachine> stateMachine;
};

}
#endif
