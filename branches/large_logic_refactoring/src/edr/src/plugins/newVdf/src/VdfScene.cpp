#include "NewVdfPCH.h"
#include "VdfScene.h"
#include "SimpleItem.h"
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsView>
#include <QtGui/QHBoxLayout>
#include "TypesWindow.h"
#include "DarkBackgroundStrategy.h"

using namespace vdf;

class SimpleBackground : public IBackgroundStrategy
{
public:
	virtual void paint(QPainter* p, const QRectF& rect)
	{
		p->fillRect(rect, QColor(77, 77, 77));
	}
};

VdfScene::VdfScene(SceneStateMachinePtr machine, SceneModelPtr sceneModel, QObject *parent /*= 0*/ ) :
    QGraphicsScene(parent), 
    sceneModel(sceneModel),
	stateMachine(machine),
	moveBlocker(false)
{
    connect(sceneModel.get(), SIGNAL(visualItemAdded(IVisualItemPtr)), this, SLOT(addVisualItem(IVisualItemPtr)));
	connect(sceneModel.get(), SIGNAL(visualItemRemoved(IVisualItemPtr)), this, SLOT(removeVisualItem(IVisualItemPtr)));
	connect(this, SIGNAL(selectionChanged()), stateMachine.get(), SLOT(selectionChanged()));
	//setBackgroundStrategy(IBackgroundStrategyPtr(new SimpleBackground()));
	setBackgroundStrategy(IBackgroundStrategyPtr(new DarkBackgroundStrategy));
}

void VdfScene::keyPressEvent(QKeyEvent *event)
{
	if (!stateMachine->keyPressEvent(event)) {
		QGraphicsScene::keyPressEvent(event);
	}
}

void VdfScene::keyReleaseEvent(QKeyEvent *event)
{
	if (!stateMachine->keyReleaseEvent(event)) {
		QGraphicsScene::keyReleaseEvent(event);
	}
}


void VdfScene::mousePressEvent( QGraphicsSceneMouseEvent *event )
{
    if (!stateMachine->mousePressEvent(event)) {
        QGraphicsScene::mousePressEvent(event);
    }
}

void VdfScene::mouseMoveEvent( QGraphicsSceneMouseEvent *event )
{
    if (!moveBlocker && !stateMachine->mouseMoveEvent(event)) {
        QGraphicsScene::mouseMoveEvent(event);
    }
	moveBlocker = false;
}

void VdfScene::mouseReleaseEvent( QGraphicsSceneMouseEvent *event )
{
    if (!stateMachine->mouseReleaseEvent(event)) {
        QGraphicsScene::mouseReleaseEvent(event);
    }
}

void VdfScene::mouseDoubleClickEvent( QGraphicsSceneMouseEvent *event )
{
    if (!stateMachine->mouseDoubleClickEvent(event)) {
        QGraphicsScene::mouseDoubleClickEvent(event);
    }
}

#ifndef QT_NO_WHEELEVENT
void VdfScene::wheelEvent( QGraphicsSceneWheelEvent *event )
{
    if (!stateMachine->wheelEvent(event)) {
        QGraphicsScene::wheelEvent(event);
    }
}
#endif

void VdfScene::dropEvent( QGraphicsSceneDragDropEvent *event )
{
    QByteArray encoded = event->mimeData()->data("application/x-qabstractitemmodeldatalist");
    QDataStream stream(&encoded, QIODevice::ReadOnly);

    // HACK - zrobic to poprawnie!
    while (!stream.atEnd())
    {
        int row, col;
        QMap<int,  QVariant> roleDataMap;
        stream >> row >> col >> roleDataMap;

        QWidget* widget = event->source();
        QObject* parent = widget->parent();
        TypesWindow* window = nullptr;
   
        while(parent && !window) {
            window = qobject_cast<TypesWindow*>(parent);
            parent = parent->parent();
        }

        if (window) {
            window->insert((roleDataMap.begin()).value().toString(), event->scenePos());
        }
    }
}

void VdfScene::dragEnterEvent( QGraphicsSceneDragDropEvent *event )
{
    //if (event->mimeData()->hasFormat("text/plain"))
        event->acceptProposedAction();
}

void VdfScene::dragMoveEvent( QGraphicsSceneDragDropEvent *event )
{

}

void VdfScene::dragLeaveEvent( QGraphicsSceneDragDropEvent *event )
{

}

void VdfScene::addVisualItem( IVisualItemPtr item )
{
	moveBlocker = true;
    addItem(item->visualItem());
}

void VdfScene::normalPressed()
{
	stateMachine->setState(stateMachine->getNormalState());
}

void VdfScene::connectPressed()
{
	//stateMachine->setState(stateMachine->getNormalState());
}

void VdfScene::groupPressed()
{
	stateMachine->setState(stateMachine->getPreGroupState());
}

void VdfScene::changeCursor( Qt::CursorShape cursor )
{
	QList<QGraphicsView*> viewList = views();
	for (auto it = viewList.begin(); it != viewList.end(); ++it) {
		(*it)->setCursor(cursor);
	}
}

void VdfScene::removeVisualItem( IVisualItemPtr item )
{
	removeItem(item->visualItem());
}

void VdfScene::drawBackground( QPainter *painter, const QRectF &rect )
{
	UTILS_ASSERT(backgroundStrategy);
	backgroundStrategy->paint(painter, rect);
}

void VdfScene::setBackgroundStrategy(IBackgroundStrategyPtr strategy)
{
	this->backgroundStrategy = strategy;
}


