#include "DicomPCH.h"
#include "MoveState.h"
#include "PointsState.h"
#include <coreui/AbstractStateMachine.h>
#include <QtGui/QGraphicsSceneMouseEvent>
#include "LayeredStateMachine.h"
#include <coreUI/MoveCommand.h>
#include "LayeredSerie.h"

using namespace dicom;

dicom::MoveState::MoveState( LayeredStateMachine* machine) :
    NormalState(machine),
    moveCursor(QPixmap(":/dicom/move.png"), 0, 0)
{

}

void MoveState::selectionChanged(const QList<QGraphicsItem*>& list)
{
    /*if (item2layer.empty()) {
    begin();
    }
    for (auto it = list.begin(); it != list.end(); ++it) {
    QGraphicsItem* item = extractItem(*it);
    auto layerIt = item2layer.find(item);
    if (layerIt != item2layer.end()) {
    (layerIt->second)->setSelected(true);
    }
    machine->getSerie()->getLayersModel()->refresh();
    }*/
    machine->setState(machine->getNormalState());
}

bool MoveState::mousePressEvent( QGraphicsSceneMouseEvent* e )
{
    position.first = nullptr;
    if (e->button() == Qt::LeftButton) {
        auto item = extractItem(e);
        if (item) {
            position = std::make_pair(item, item->pos());
        }
    }

    // celowo zwracany jest false, nawet gdy obs³u¿yliœmy event (domyœlna obs³uga jest w tym przypadku porz¹dana)
    return false;
}

bool MoveState::mouseReleaseEvent( QGraphicsSceneMouseEvent* e )
{
    if (e->button() == Qt::LeftButton) {
        if (position.first != nullptr) {
            QGraphicsItem* item = position.first;
            QPointF oldP = position.second;
            QPointF newP = item->pos();
            machine->getCommandStack()->addCommand(utils::make_shared<coreUI::MoveCommand>(item, newP, oldP));
        }
    }
    return false;
}



void MoveState::end()
{
    NormalState::end();
    machine->changeCursor(Qt::ArrowCursor);
    position.first = nullptr;
}

void dicom::MoveState::begin()
{
    NormalState::begin();
    machine->changeCursor(moveCursor);
}

bool dicom::MoveState::mouseMoveEvent( QGraphicsSceneMouseEvent* e )
{
    return false;
}
