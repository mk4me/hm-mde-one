#include "DicomPCH.h"
#include "NormalState.h"
#include "PointsState.h"
#include <coreui/AbstractStateMachine.h>
#include <QtGui/QGraphicsSceneMouseEvent>
#include "LayeredStateMachine.h"
#include <coreUI/MoveCommand.h>

using namespace dicom;

dicom::NormalState::NormalState( LayeredStateMachine* machine ) :
    coreUI::AbstractState(machine),
    machine(machine)
{

}

void NormalState::selectionChanged(const QList<QGraphicsItem*>& list)
{
//    auto nodes = stateMachine->getSceneModel()->getVisualItems<IVisualNodePtr>();
//    for (auto it = nodes.begin(); it != nodes.end(); ++it) {
//        IVisualNodePtr node = *it;
//        node->setSelection(node->visualItem()->isSelected());
//    }
//    if (list.size() == 1) {
//        IVisualItemPtr item = stateMachine->getSceneModel()->tryGetVisualItem(list.first());
//        if (item && item->isType(IVisualItem::Pin)) {
//            IVisualPinPtr pin = utils::dynamic_pointer_cast<IVisualPin>(item);
//            ConnectStatePtr connectState = stateMachine->getConnectState();
//            connectState->setFirstPin(pin);
//            stateMachine->setState(stateMachine->getConnectState());
//        }
//    }
}

bool NormalState::mousePressEvent( QGraphicsSceneMouseEvent* e )
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

bool NormalState::mouseReleaseEvent( QGraphicsSceneMouseEvent* e )
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

bool NormalState::keyReleaseEvent( QKeyEvent *event )
{
    /*if (event->key() == Qt::Key_Delete) {
    auto selected = stateMachine->getScene()->selectedItems();
    auto command = utils::ICommandPtr(new RemoveSelectedCommand(stateMachine->getSceneModel(), selected));
    stateMachine->getCommandStack()->addCommand(command);
    return true;
    } */

    return false;
}


void NormalState::end()
{
    position.first = nullptr;
}

QGraphicsItem* NormalState::extractItem( QGraphicsSceneMouseEvent* e )
{
    QGraphicsItem* itm = machine->getGraphicsScene()->itemAt(e->scenePos());
    if (itm) {
        QGraphicsItemGroup* grp = dynamic_cast<QGraphicsItemGroup*>(itm->parentItem());
        if (grp) {
            return grp;
        }
    }

    return itm;
}


