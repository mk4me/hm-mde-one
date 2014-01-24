#include "DicomPCH.h"
#include "LayeredStateMachine.h"
#include "LayeredSerie.h"

using namespace dicom;

dicom::LayeredStateMachine::LayeredStateMachine(LayeredSerie* serie, utils::ICommandStackPtr commandStack) :
    normalState(new NormalState(this)),
    //pointsState(new PointsState(this, true)),
    //polyState(new PointsState(this, false)),
    boneState(new PointsState(this, true, true, adnotations::bone)),
    skinState(new PointsState(this, false, true, adnotations::skin)),
    tendonState(new PointsState(this, true, true, adnotations::tendon)),
    jointState(new PointsState(this, false, false, adnotations::joint)),
    inflamatoryState(new PointsState(this, true, false, adnotations::inflammatory)),
    noiseState(new PointsState(this, false, false, adnotations::noise)),
    editState(new EditState(this)),
    moveState(new MoveState(this)),
    serie(serie),
    commandStack(commandStack)
{
    addState(normalState);
    //addState(pointsState);
    //addState(polyState);
    addState(boneState);
    addState(skinState);
    addState(tendonState);
    addState(jointState);
    addState(inflamatoryState);
    addState(noiseState);

    addState(editState);
    addState(moveState);
}

dicom::NormalStatePtr dicom::LayeredStateMachine::getNormalState()
{
    return normalState;
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
    bool b = getGraphicsScene()->signalsBlocked();
    getGraphicsScene()->blockSignals(true);
    getCurrentState()->selectionChanged(items);
    getGraphicsScene()->blockSignals(b);
}

EditStatePtr dicom::LayeredStateMachine::getEditState()
{
    return editState;
}

void dicom::LayeredStateMachine::changeCursor( const QCursor& cursor )
{
    QList<QGraphicsView*> viewList = getGraphicsScene()->views();
    for (auto it = viewList.begin(); it != viewList.end(); ++it) {
        (*it)->setCursor(cursor);
    }
}

dicom::MoveStatePtr dicom::LayeredStateMachine::getMoveState()
{
    return moveState;
}

