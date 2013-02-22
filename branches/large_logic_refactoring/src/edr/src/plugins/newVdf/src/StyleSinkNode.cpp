#include "NewVdfPCH.h"
#include "StyleSinkNode.h"
#include "StyleItem.h"

using namespace vdf;

StyleSinkNode::StyleSinkNode() : 
	styleItem(new StyleItem)
{

}


void StyleSinkNode::addInputPin( IVisualInputPinPtr pin )
{
    pin->visualItem()->setParentItem(styleItem);
	INodeStrategyPtr nodeStr = core::dynamic_pointer_cast<INodeStrategy>(styleItem->getStrategy());
	pin->visualItem()->setPos(nodeStr->getPinPosition(pins.size(), true));
    pins.push_back(pin);
}

void StyleSinkNode::removeInputPin( IVisualInputPinPtr pin )
{
    throw std::exception("The method or operation is not implemented.");
}

void StyleSinkNode::clearInputPins()
{
    throw std::exception("The method or operation is not implemented.");
}

void StyleSinkNode::setName( const QString & name )
{
	this->name = name;
    styleItem->getStrategy()->update();
}

void StyleSinkNode::setConfigButton( QAbstractButton * button )
{
    throw std::exception("The method or operation is not implemented.");
}

void StyleSinkNode::setCloseButton( QAbstractButton * button )
{
    throw std::exception("The method or operation is not implemented.");
}

QGraphicsItem * StyleSinkNode::visualItem() const
{
    return styleItem;
}

void StyleSinkNode::addSelection()
{
    //styleItem->setSelected(true);
	strategy->setState(INodeStrategy::Selected);
}

void StyleSinkNode::removeSelection()
{
    //styleItem->setSelected(false);
	strategy->setState(INodeStrategy::Normal);
}

void StyleSinkNode::addHover()
{
    throw std::exception("The method or operation is not implemented.");
}

void StyleSinkNode::removeHover()
{
    throw std::exception("The method or operation is not implemented.");
}

void StyleSinkNode::addCollision()
{
    throw std::exception("The method or operation is not implemented.");
}

void StyleSinkNode::removeCollision()
{
    throw std::exception("The method or operation is not implemented.");
}

void StyleSinkNode::setVisualStrategy( IVisualStrategyPtr strategy )
{
	this->strategy = core::dynamic_pointer_cast<INodeStrategy>(strategy);
	styleItem->setStrategy(strategy);
}

QString StyleSinkNode::getName() const
{
	return name;
}


