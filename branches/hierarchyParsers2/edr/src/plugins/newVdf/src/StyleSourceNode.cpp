#include "NewVdfPCH.h"
#include "StyleSourceNode.h"
#include <plugins/newVdf/INodeStrategy.h>
#include <plugins/newVdf/IVisualOutputPin.h>
#include "StyleItem.h"

using namespace vdf;

StyleSourceNode::StyleSourceNode() :
    styleItem(new StyleItem())
{

}

void StyleSourceNode::addOutputPin( IVisualOutputPinPtr pin )
{
    pin->visualItem()->setParentItem(styleItem);
    INodeStrategyPtr nodeStr = utils::dynamic_pointer_cast<INodeStrategy>(styleItem->getStrategy());
	pin->visualItem()->setPos(nodeStr->getPinPosition(pins.size(), false));
	pins.push_back(pin);
}


void StyleSourceNode::clearOutputPins()
{
    throw std::runtime_error("The method or operation is not implemented.");
}

void StyleSourceNode::setName( const QString & name )
{
	this->name = name;
    styleItem->getStrategy()->update();
}

void StyleSourceNode::setConfigButton( QAbstractButton * button )
{
    throw std::runtime_error("The method or operation is not implemented.");
}

void StyleSourceNode::setCloseButton( QAbstractButton * button )
{
    throw std::runtime_error("The method or operation is not implemented.");
}

QGraphicsItem * StyleSourceNode::visualItem() const
{
    return styleItem;
}

void StyleSourceNode::addSelection()
{
    styleItem->setSelected(true);
	strategy->setState(INodeStrategy::Selected);
    styleItem->setZValue(Z<IVisualItem::Node, true>::value());
}

void StyleSourceNode::removeSelection()
{
    styleItem->setSelected(false);
	strategy->setState(INodeStrategy::Normal);
    styleItem->setZValue(Z<IVisualItem::Node, false>::value());
}

void StyleSourceNode::addHover()
{
    throw std::runtime_error("The method or operation is not implemented.");
}

void StyleSourceNode::removeHover()
{
    throw std::runtime_error("The method or operation is not implemented.");
}

void StyleSourceNode::addCollision()
{
    throw std::runtime_error("The method or operation is not implemented.");
}

void StyleSourceNode::removeCollision()
{
    throw std::runtime_error("The method or operation is not implemented.");
}

void StyleSourceNode::removeOutputPin( IVisualOutputPinPtr pin )
{
	UTILS_ASSERT(false);
}

void StyleSourceNode::setVisualStrategy( IVisualStrategyPtr strategy )
{
	this->strategy = utils::dynamic_pointer_cast<INodeStrategy>(strategy);
	UTILS_ASSERT(this->strategy);
	styleItem->setStrategy(strategy);
}

QString StyleSourceNode::getName() const 
{
	return name;
}
