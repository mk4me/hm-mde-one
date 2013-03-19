#include "NewVdfPCH.h"
#include "StyleProcessingNode.h"
#include <plugins/newVdf/INodeStrategy.h>
#include <plugins/newVdf/IVisualInputPin.h>
#include <plugins/newVdf/IVisualOutputPin.h>
#include "StyleItem.h"

using namespace vdf;

StyleProcessingNode::StyleProcessingNode() :
    styleItem(new StyleItem())
{

}

void StyleProcessingNode::addInputPin( IVisualInputPinPtr pin )
{
    //pin->visualItem()->setPos(margin, margin + inputPins.size() * pinHeight);
    pin->visualItem()->setParentItem(styleItem);

	INodeStrategyPtr nodeStr = core::dynamic_pointer_cast<INodeStrategy>(styleItem->getStrategy());
	pin->visualItem()->setPos(nodeStr->getPinPosition(inputPins.size(), true));

    inputPins.push_back(pin);
    //StyleItem->setSize(calculateNodeSize(inputPins.size(), outputPins.size()));
}

void StyleProcessingNode::removeInputPin( IVisualInputPinPtr pin )
{
    throw std::exception("The method or operation is not implemented.");
}

void StyleProcessingNode::clearInputPins()
{
    throw std::exception("The method or operation is not implemented.");
}

void StyleProcessingNode::setName( const QString & name )
{
    this->name = name;
	this->styleItem->getStrategy()->update();
}

void StyleProcessingNode::setConfigButton( QAbstractButton * button )
{
    throw std::exception("The method or operation is not implemented.");
}

void StyleProcessingNode::setCloseButton( QAbstractButton * button )
{
    throw std::exception("The method or operation is not implemented.");
}

QGraphicsItem * StyleProcessingNode::visualItem() const
{
    return styleItem;
}

void StyleProcessingNode::addSelection()
{
	strategy->setState(INodeStrategy::Selected);
    styleItem->setSelected(true);
    styleItem->setZValue(Z<IVisualItem::Node, true>::value());
}

void StyleProcessingNode::removeSelection()
{
	strategy->setState(INodeStrategy::Normal);
    styleItem->setSelected(false);
    styleItem->setZValue(Z<IVisualItem::Node, false>::value());
}

void StyleProcessingNode::addHover()
{
    throw std::exception("The method or operation is not implemented.");
}

void StyleProcessingNode::removeHover()
{
    throw std::exception("The method or operation is not implemented.");
}

void StyleProcessingNode::addCollision()
{
    throw std::exception("The method or operation is not implemented.");
}

void StyleProcessingNode::removeCollision()
{
    throw std::exception("The method or operation is not implemented.");
}

void StyleProcessingNode::addOutputPin( IVisualOutputPinPtr pin )
{
    pin->visualItem()->setParentItem(styleItem);
    //pin->visualItem()->setPos(margin + nodeWidth - pin2, margin + outputPins.size() * pinHeight);

	INodeStrategyPtr nodeStr = core::dynamic_pointer_cast<INodeStrategy>(styleItem->getStrategy());
	pin->visualItem()->setPos(nodeStr->getPinPosition(outputPins.size(), false));

    outputPins.push_back(pin);
    //StyleItem->setSize(calculateNodeSize(inputPins.size(), outputPins.size()));
}

void StyleProcessingNode::removeOutputPin( IVisualOutputPinPtr pin )
{
    throw std::exception("The method or operation is not implemented.");
}

void StyleProcessingNode::clearOutputPins()
{
    throw std::exception("The method or operation is not implemented.");
}

void StyleProcessingNode::setVisualStrategy( IVisualStrategyPtr strategy )
{
	UTILS_ASSERT(styleItem && core::dynamic_pointer_cast<INodeStrategy>(strategy));
	this->strategy = core::dynamic_pointer_cast<INodeStrategy>(strategy);
	styleItem->setStrategy(this->strategy);
}

QString StyleProcessingNode::getName() const 
{
	return name;
}

