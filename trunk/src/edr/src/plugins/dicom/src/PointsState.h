/********************************************************************
	created:	2013/11/13
	created:	13:11:2013   16:46
	filename: 	PointsState.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__POINTSSTATE_H__
#define HEADER_GUARD_DICOM__POINTSSTATE_H__

#include <coreui/AbstractState.h>
#include "PointsLayer.h"
namespace dicom {

class LayeredStateMachine;

class PointsState : public QObject, public coreUI::AbstractState
{
    Q_OBJECT
public:
    PointsState(LayeredStateMachine* machine, bool curved);
	virtual ~PointsState() {}
    friend class AddLayerCommand;
public:
    virtual bool keyReleaseEvent( QKeyEvent *event );
    virtual bool mouseMoveEvent(QGraphicsSceneMouseEvent* e);
    virtual bool mouseReleaseEvent( QGraphicsSceneMouseEvent* e );
    virtual bool mousePressEvent( QGraphicsSceneMouseEvent* e );

private Q_SLOTS:
    void clear();

    void resetLayer();

    void addLayer();

    virtual void begin( coreUI::AbstractStateConstPtr lastState );
    virtual void end();


private:
    std::vector<std::pair<QGraphicsItem*, QPointF>> positionsToCheck;
    PointsLayerPtr layer;
    LayeredStateMachine* machine;
    bool possibleMove;
    bool curved;
};
DEFINE_SMART_POINTERS(PointsState);

}
#endif
