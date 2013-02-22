#include "NewVdfPCH.h"
#include "StyleInputPinNode.h"
#include "StyleItem.h"

using namespace vdf;

StyleInputPinNode::StyleInputPinNode() :
    item(new StyleItem()),
	index(-1)
{

}

StyleInputPinNode::~StyleInputPinNode()
{

}


void StyleInputPinNode::markRequired()
{
    throw std::exception("The method or operation is not implemented.");
}

void StyleInputPinNode::markMissing()
{
    throw std::exception("The method or operation is not implemented.");
}

void StyleInputPinNode::markNormal()
{
	strategy->setState(IPinStrategy::Normal);
	item->update();
}

void StyleInputPinNode::markCompatible()
{
    throw std::exception("The method or operation is not implemented.");
}

void StyleInputPinNode::markIncompatible()
{
    throw std::exception("The method or operation is not implemented.");
}

void StyleInputPinNode::markConnective()
{
	strategy->setState(IPinStrategy::Connective);
	item->update();
}

void StyleInputPinNode::markUnconnective()
{

	strategy->setState(IPinStrategy::Unconnective);
    //item->setColor(QColor(230, 0, 0));
	item->update();
}

void StyleInputPinNode::markConnected()
{
	strategy->setState(IPinStrategy::Connected);
}

QGraphicsItem * StyleInputPinNode::visualItem() const
{
    return item;
}

void StyleInputPinNode::addSelection()
{
    throw std::exception("The method or operation is not implemented.");
}

void StyleInputPinNode::removeSelection()
{
    throw std::exception("The method or operation is not implemented.");
}

void StyleInputPinNode::addHover()
{
    throw std::exception("The method or operation is not implemented.");
}

void StyleInputPinNode::removeHover()
{
    throw std::exception("The method or operation is not implemented.");
}

void StyleInputPinNode::addCollision()
{
    throw std::exception("The method or operation is not implemented.");
}

void StyleInputPinNode::removeCollision()
{
    throw std::exception("The method or operation is not implemented.");
}

void StyleInputPinNode::setVisualStrategy( IVisualStrategyPtr strategy )
{
	this->strategy = core::dynamic_pointer_cast<IPinStrategy>(strategy);
	item->setStrategy(strategy);
}



