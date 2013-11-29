#include "DicomPCH.h"
#include "LayeredStateMachine.h"
#include "LayeredSerie.h"

using namespace dicom;

dicom::LayeredStateMachine::LayeredStateMachine(LayeredSerie* serie, utils::ICommandStackPtr commandStack) :
    normalState(new NormalState(this)),
    pointsState(new PointsState(this, true)),
    polyState(new PointsState(this, false)),
    editState(new EditState(this)),
    serie(serie),
    commandStack(commandStack)
{
    addState(normalState);
    addState(pointsState);
    addState(polyState);
    addState(editState);
}

dicom::NormalStatePtr dicom::LayeredStateMachine::getNormalState()
{
    return normalState;
}

dicom::PointsStatePtr dicom::LayeredStateMachine::getCurveState()
{
    return pointsState;
}

dicom::PointsStatePtr dicom::LayeredStateMachine::getPolyState()
{
    return polyState;
}


QGraphicsScene* dicom::LayeredStateMachine::getGraphicsScene()
{
    return serie->getGraphicsScene();
}

QGraphicsView* dicom::LayeredStateMachine::getGraphicsView()
{
    return serie->getGraphicsView();
}

void dicom::LayeredStateMachine::selectionChanged()
{
    auto items = getGraphicsScene()->selectedItems();
    getCurrentState()->selectionChanged(items);
}

EditStatePtr dicom::LayeredStateMachine::getEditState()
{
    return editState;
}

