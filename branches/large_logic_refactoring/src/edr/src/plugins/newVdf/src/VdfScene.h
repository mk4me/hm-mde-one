/********************************************************************
	created:	2012/12/20
	created:	20:12:2012   13:16
	filename: 	VdfScene.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__VDFSCENE_H__
#define HEADER_GUARD_NEWVDF__VDFSCENE_H__

#include <QtGui/QWidget>
#include <QtGui/QGraphicsView>
#include <QtGui/QWheelEvent>
#include <plugins/newVdf/IBackgroundStrategy.h>

#include "SceneStateMachine.h"
#include "SceneModel.h"

class QGraphicsScene;
class IVisualItem;
class QMouseEvent;

namespace vdf {
class SceneStateMachine;

class VdfScene : public QGraphicsScene
{
    Q_OBJECT;
public:
    VdfScene(SceneStateMachinePtr machine, SceneModelPtr sceneModel, QObject *parent = 0);
	virtual ~VdfScene() {}

public:
    virtual void mousePressEvent( QGraphicsSceneMouseEvent *event );
    virtual void mouseReleaseEvent( QGraphicsSceneMouseEvent *event );
    virtual void mouseDoubleClickEvent( QGraphicsSceneMouseEvent *event );
    virtual void mouseMoveEvent( QGraphicsSceneMouseEvent *event );
    virtual void wheelEvent( QGraphicsSceneWheelEvent *event );
    virtual void dropEvent(QGraphicsSceneDragDropEvent *event);
    virtual void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    virtual void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
    virtual void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);
	virtual void keyPressEvent(QKeyEvent *event);
	virtual void keyReleaseEvent(QKeyEvent *event);

public slots:
    void addVisualItem(IVisualItemPtr item);
	void removeVisualItem(IVisualItemPtr item);
	void normalPressed();
	void connectPressed();
	void groupPressed();
	void setBackgroundStrategy(IBackgroundStrategyPtr strategy);

public:
    SceneModelPtr getSceneModel() const { UTILS_ASSERT(sceneModel); return sceneModel; }
	void changeCursor(Qt::CursorShape cursor);


protected:
	virtual void drawBackground( QPainter *painter, const QRectF &rect );

private:
    SceneStateMachinePtr stateMachine;
    SceneModelPtr sceneModel;
	//! flaga blokuj¹ca zdarzenie MouseMove gdy dodawany jest element do sceny.
	//! Qt musi taki element i jego childy zaindeksowaæ w drzewie bsp, 
	//! wygl¹da jendak na to, ¿e robi to iteracyjnie, dlatego drag&drop mo¿e wyzwoliæ asercjê
	bool moveBlocker;
	IBackgroundStrategyPtr backgroundStrategy;
};

}

#endif
