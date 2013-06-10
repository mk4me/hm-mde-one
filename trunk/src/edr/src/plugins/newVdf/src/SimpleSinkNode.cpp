#include "NewVdfPCH.h"
#include "SimpleSinkNode.h"
#include "SimpleItem.h"

using namespace vdf;

SimpleSinkNode::SimpleSinkNode() : 
    simpleItem(new SimpleTextItem (0,0))
{
}


void SimpleSinkNode::addInputPin( IVisualInputPinPtr pin )
{
    pin->visualItem()->setParentItem(simpleItem);
    pin->visualItem()->setPos(margin, margin + pins.size() * pinHeight);
    pins.push_back(pin);
    simpleItem->setSize(calculateNodeSize(pins.size()));
}

void SimpleSinkNode::removeInputPin( IVisualInputPinPtr pin )
{
    throw std::runtime_error("The method or operation is not implemented.");
}

void SimpleSinkNode::clearInputPins()
{
    throw std::runtime_error("The method or operation is not implemented.");
}

void SimpleSinkNode::setName( const QString & name )
{
	this->name = name;
    simpleItem->setText(name);
}

void SimpleSinkNode::setConfigButton( QAbstractButton * button )
{
    throw std::runtime_error("The method or operation is not implemented.");
}

void SimpleSinkNode::setCloseButton( QAbstractButton * button )
{
    throw std::runtime_error("The method or operation is not implemented.");
}

QGraphicsItem * SimpleSinkNode::visualItem() const
{
    return simpleItem;
}

void SimpleSinkNode::addSelection()
{
    simpleItem->setSelected(true);
}

void SimpleSinkNode::removeSelection()
{
    simpleItem->setSelected(false);
}

void SimpleSinkNode::addHover()
{
    throw std::runtime_error("The method or operation is not implemented.");
}

void SimpleSinkNode::removeHover()
{
    throw std::runtime_error("The method or operation is not implemented.");
}

void SimpleSinkNode::addCollision()
{
    throw std::runtime_error("The method or operation is not implemented.");
}

void SimpleSinkNode::removeCollision()
{
    throw std::runtime_error("The method or operation is not implemented.");
}

void SimpleSinkNode::setVisualStrategy( IVisualStrategyPtr strategy )
{
	throw std::runtime_error("The method or operation is not implemented.");
}

QString SimpleSinkNode::getName() const
{
	return name;
}


