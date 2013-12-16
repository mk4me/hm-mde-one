/********************************************************************
	created:	2013/11/13
	created:	13:11:2013   16:45
	filename: 	NormalState.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__NORMALSTATE_H__
#define HEADER_GUARD_DICOM__NORMALSTATE_H__

#include <coreui/AbstractState.h>
#include <plugins/dicom/ILayeredImage.h>

namespace dicom {

class LayeredStateMachine;

class NormalState : public QObject, public coreUI::AbstractState
{
    Q_OBJECT
public:
    NormalState(LayeredStateMachine* machine);
	virtual ~NormalState() {}

public:
    virtual void selectionChanged(const QList<QGraphicsItem*>& list);
    virtual bool mousePressEvent(QGraphicsSceneMouseEvent* e);
    virtual bool mouseMoveEvent(QGraphicsSceneMouseEvent* e);
    virtual bool mouseReleaseEvent(QGraphicsSceneMouseEvent* e);
    virtual bool keyReleaseEvent( QKeyEvent *event );
    virtual void begin();


    virtual void end();

protected:
    int getNumSelected();
    ILayerItemPtr getFirstSelected();
    void createItem2LayerMap();
    QGraphicsItem* extractItem( QGraphicsSceneMouseEvent* e );
    QGraphicsItem* extractItem( QGraphicsItem* itm);

protected: 
    LayeredStateMachine* machine;
    std::map<QGraphicsItem*, ILayerItemPtr> item2layer;

private Q_SLOTS:
    void move();
    void addCurve();
    void addPoly();
    void edit();
};
DEFINE_SMART_POINTERS(NormalState);

}
#endif
