/********************************************************************
	created:	2012/12/20
	created:	20:12:2012   13:16
	filename: 	VdfScene.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__VDFSCENE_H__
#define HEADER_GUARD_NEWVDF__VDFSCENE_H__

#include <QtWidgets/QWidget>
#include <QtWidgets/QGraphicsView>
#include <QtGui/QWheelEvent>
#include <plugins/newVdf/IBackgroundStrategy.h>

#include "SceneStateMachine.h"
#include "SceneModel.h"
#include "TypesModel.h"

class QGraphicsScene;
class IVisualItem;
class QMouseEvent;

namespace vdf {
class SceneStateMachine;

//! Scena vdf, obs�uguje IO
class VdfScene : public QGraphicsScene
{
    Q_OBJECT;
public:
    //! Konstruktor.
    //! \param machine 
    //! \param sceneModel 
    //! \param parent 
    VdfScene(SceneStateMachinePtr machine, SceneModelPtr sceneModel, TypesModelPtr typesModel, QObject *parent = 0);
	virtual ~VdfScene() {}

public:
    //! 
    //! \param event 
    virtual void mousePressEvent( QGraphicsSceneMouseEvent *event );
    //! 
    //! \param event 
    virtual void mouseReleaseEvent( QGraphicsSceneMouseEvent *event );
    //! 
    //! \param event 
    virtual void mouseDoubleClickEvent( QGraphicsSceneMouseEvent *event );
    //! 
    //! \param event 
    virtual void mouseMoveEvent( QGraphicsSceneMouseEvent *event );
    //! 
    //! \param event 
    virtual void wheelEvent( QGraphicsSceneWheelEvent *event );
    //! 
    //! \param event 
    virtual void dropEvent(QGraphicsSceneDragDropEvent *event);
    //! 
    //! \param event 
    virtual void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    //! 
    //! \param event 
    virtual void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
    //! 
    //! \param event 
    virtual void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);
	//! 
	//! \param event 
	virtual void keyPressEvent(QKeyEvent *event);
	//! 
	//! \param event 
	virtual void keyReleaseEvent(QKeyEvent *event);

public slots:
    //! Dodaje element do sceny
    //! \param item Obiekt przechowuj�cy element do dodania
    void addVisualItem(IVisualItemPtr item);
	//! Usuwa element ze sceny 
	//! \param item Obiekt przechowuj�cy element do usuni�cia
	void removeVisualItem(IVisualItemPtr item);
	//! Pozwala zmieni� t�o
	//! \param strategy obiekt ze strategi� nowego t�a
	void setBackgroundStrategy(IBackgroundStrategyPtr strategy);

public:
    //! \return model sceny
    SceneModelPtr getSceneModel() const { UTILS_ASSERT(sceneModel); return sceneModel; }
	//! Zmiana kursora dla wszystkich widok�w sceny
	//! \param cursor nowy kursor myszy
	void changeCursor(Qt::CursorShape cursor);


protected:
	//! Odrysowuje t�o sceny
	//! \param painter 
	//! \param rect 
	virtual void drawBackground( QPainter *painter, const QRectF &rect );

private:
    //! obiekt obs�uguje stany sceny (np. zaznaczanie, przesuwanie, ��czenie element�w sceny)
    SceneStateMachinePtr stateMachine;
    //! model sceny
    SceneModelPtr sceneModel;
	//! obsluguje typy elementow
	TypesModelPtr typesModel;
	//! flaga blokuj�ca zdarzenie MouseMove gdy dodawany jest element do sceny.
	//! Qt musi taki element i jego childy zaindeksowa� w drzewie bsp, 
	//! wygl�da jendak na to, �e robi to iteracyjnie, dlatego drag&drop mo�e wyzwoli� asercj�
	bool moveBlocker;
    //! obiekt odrysowuj�cy t�o sceny
	IBackgroundStrategyPtr backgroundStrategy;
};

}

#endif
