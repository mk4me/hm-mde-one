#include "NewVdfPCH.h"
#include "NormalState.h"
#include "SimpleItem.h"
#include "SceneStateMachine.h"
#include "VdfScene.h"

using namespace vdf;

void NormalState::selectionChanged(const QList<QGraphicsItem*>& list)
{
	auto nodes = stateMachine->getSceneModel()->getVisualItems<IVisualNodePtr>();
	for (auto it = nodes.begin(); it != nodes.end(); ++it) {
		IVisualNodePtr node = *it;
		if (node->visualItem()->isSelected()) {
			node->addSelection();
		} else {
			node->removeSelection();
		}
	}
    if (list.size() == 1) {
		IVisualItemPtr item = stateMachine->getSceneModel()->tryGetVisualItem(list.first());
        if (item && item->isType(IVisualItem::Pin)) {
            pin = core::dynamic_pointer_cast<IVisualPin>(item);
            stateMachine->setState(stateMachine->getConnectState());
        }
    }
}

bool NormalState::mousePressEvent( QGraphicsSceneMouseEvent* e )
{
	if (e->button() == Qt::LeftButton) {
		if (stateMachine->getScene()->itemAt(e->scenePos())) {
			positions.clear();
			auto nodes = stateMachine->getSceneModel()->getVisualItems<IVisualNodePtr>();
			for (auto it = nodes.begin(); it != nodes.end(); ++it) {
				positions[*it] = (*it)->visualItem()->scenePos();
			}
		} else {
			stateMachine->setState(stateMachine->getGroupState());
		}
		
	}

	// celowo zwracany jest false, nawet gdy obs�u�yli�my event (domy�lna obs�uga jest w tym przypadku porz�dana)
	return false;
}

bool NormalState::mouseReleaseEvent( QGraphicsSceneMouseEvent* e )
{

	if (e->button() == Qt::LeftButton) {
		std::vector<ICommandPtr> commands;
		for (auto it = positions.begin(); it != positions.end(); ++it) {
			QGraphicsItem* item = it->first->visualItem();
			QPointF oldP = it->second;
			QPointF newP = item->pos();
			if (newP != oldP) {
				commands.push_back(ICommandPtr(new MoveCommand(item, newP, oldP)));
			}
		}
		if (!commands.empty()) {
			stateMachine->getCommandStack()->addCommand(ICommandPtr(new MultiCommand(commands)));
		}
		//return true;
	}
	return false;
}

bool NormalState::keyReleaseEvent( QKeyEvent *event )
{
	if (event->key() == Qt::Key_Delete) {
		auto selected = stateMachine->getScene()->selectedItems();
		auto nodes = stateMachine->getSceneModel()->getVisualItems<IVisualNodePtr>(selected);

		std::vector<ICommandPtr> commands;
		for (auto it = nodes.begin(); it != nodes.end(); ++it) {
			commands.push_back(ICommandPtr(new RemoveCommand(stateMachine->getSceneModel(), *it)));
		}
		if (!commands.empty()) {
			stateMachine->getCommandStack()->addCommand(ICommandPtr(new MultiCommand(commands)));
		}
		return true;
	}

	return false;
}

