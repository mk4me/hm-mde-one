/********************************************************************
	created:	2013/11/13
	created:	13:11:2013   11:25
	filename: 	LayeredStateMachine.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__LAYEREDSTATEMACHINE_H__
#define HEADER_GUARD_DICOM__LAYEREDSTATEMACHINE_H__

#include <coreui/AbstractStateMachine.h>
#include "NormalState.h"
#include "EditState.h"
#include "PointsState.h"
#include <utils/ICommandStack.h>

namespace dicom {

class LayeredSerie;

class LayeredStateMachine : public coreUI::AbstractStateMachine
{
    Q_OBJECT
public:
    LayeredStateMachine(LayeredSerie* serie, utils::ICommandStackPtr commandStack);
	virtual ~LayeredStateMachine() {}

public:
    NormalStatePtr getNormalState();
    PointsStatePtr getCurveState();
    PointsStatePtr getPolyState();
    EditStatePtr getEditState();
    LayeredSerie* getSerie() const { return serie; }
    QGraphicsScene* getGraphicsScene();
    QGraphicsView* getGraphicsView();
    utils::ICommandStackPtr getCommandStack() const { return commandStack; }

public Q_SLOTS:
    void selectionChanged();

private:
    NormalStatePtr normalState;
    EditStatePtr editState;
    PointsStatePtr pointsState;
    PointsStatePtr polyState;
    LayeredSerie* serie;
    utils::ICommandStackPtr commandStack;
};
DEFINE_SMART_POINTERS(LayeredStateMachine);

}
#endif
