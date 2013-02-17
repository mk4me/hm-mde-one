#include "NewVdfPCH.h"
#include "SimpleOutputPinNode.h"
#include "SimpleItem.h"

SimpleOutputPinNode::SimpleOutputPinNode() :
	item(new SimplePinItem()),
	index(-1)
{

}

SimpleOutputPinNode::~SimpleOutputPinNode()
{

}


void SimpleOutputPinNode::markIncomplete()
{
    throw std::exception("The method or operation is not implemented.");
}

void SimpleOutputPinNode::markNormal()
{
	item->setColor(QColor(180, 180, 180));
	item->update();
}

void SimpleOutputPinNode::markCompatible()
{
    throw std::exception("The method or operation is not implemented.");
}

void SimpleOutputPinNode::markIncompatible()
{
    throw std::exception("The method or operation is not implemented.");
}

void SimpleOutputPinNode::markConnective()
{
	item->setColor(QColor(50, 50, 200));
	item->update();
}

void SimpleOutputPinNode::markUnconnective()
{
	item->setColor(QColor(230, 0, 0));
	item->update();
}

void SimpleOutputPinNode::markConnected()
{
    throw std::exception("The method or operation is not implemented.");
}

QGraphicsItem * SimpleOutputPinNode::visualItem() const
{
    return item;
}

void SimpleOutputPinNode::addSelection()
{
    throw std::exception("The method or operation is not implemented.");
}

void SimpleOutputPinNode::removeSelection()
{
    throw std::exception("The method or operation is not implemented.");
}

void SimpleOutputPinNode::addHover()
{
    throw std::exception("The method or operation is not implemented.");
}

void SimpleOutputPinNode::removeHover()
{
    throw std::exception("The method or operation is not implemented.");
}

void SimpleOutputPinNode::addCollision()
{
    throw std::exception("The method or operation is not implemented.");
}

void SimpleOutputPinNode::removeCollision()
{
    throw std::exception("The method or operation is not implemented.");
}

void SimpleOutputPinNode::setVisualStrategy( IVisualStrategyPtr strategy )
{
	throw std::exception("The method or operation is not implemented.");
}
