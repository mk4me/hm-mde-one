#include "CoreUiPCH.h"
#include <coreui/MoveCommand.h>
#include <QtGui/QGraphicsItem>

using namespace coreUI;

MoveCommand::MoveCommand( QGraphicsItem* item, const QPointF& newP, const QPointF& oldP ) :
item(item),
    newPosition(newP),
    oldPosition(oldP)
{

}

void MoveCommand::doIt()
{
    item->setPos(newPosition);
}

void MoveCommand::undoIt()
{
    item->setPos(oldPosition);
}