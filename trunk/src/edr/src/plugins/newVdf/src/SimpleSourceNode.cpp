#include "NewVdfPCH.h"
#include "SimpleSourceNode.h"
#include <plugins/newVdf/IVisualOutputPin.h>
#include "SimpleItem.h"

using namespace vdf;

SimpleSourceNode::SimpleSourceNode() :
    simpleItem(new SimpleTextItem(0, 0))
{

}

void SimpleSourceNode::addOutputPin( IVisualOutputPinPtr pin )
{
    pin->visualItem()->setParentItem(simpleItem);
    pin->visualItem()->setPos(margin + nodeWidth - pin2, margin + pins.size() * pinHeight);
    pins.push_back(pin);
    simpleItem->setSize(calculateNodeSize(pins.size()));
}


void SimpleSourceNode::clearOutputPins()
{
    throw std::exception("The method or operation is not implemented.");
}

void SimpleSourceNode::setName( const QString & name )
{
	this->name = name;
    simpleItem->setText(name);
}

void SimpleSourceNode::setConfigButton( QAbstractButton * button )
{
    throw std::exception("The method or operation is not implemented.");
}

void SimpleSourceNode::setCloseButton( QAbstractButton * button )
{
    throw std::exception("The method or operation is not implemented.");
}

QGraphicsItem * SimpleSourceNode::visualItem() const
{
    return simpleItem;
}

void SimpleSourceNode::addSelection()
{
    simpleItem->setSelected(true);
}

void SimpleSourceNode::removeSelection()
{
    simpleItem->setSelected(false);
}

void SimpleSourceNode::addHover()
{
    throw std::exception("The method or operation is not implemented.");
}

void SimpleSourceNode::removeHover()
{
    throw std::exception("The method or operation is not implemented.");
}

void SimpleSourceNode::addCollision()
{
    throw std::exception("The method or operation is not implemented.");
}

void SimpleSourceNode::removeCollision()
{
    throw std::exception("The method or operation is not implemented.");
}

void SimpleSourceNode::removeOutputPin( IVisualOutputPinPtr pin )
{

}

void SimpleSourceNode::setVisualStrategy( IVisualStrategyPtr strategy )
{
	throw std::exception("The method or operation is not implemented.");
}

QString SimpleSourceNode::getName() const
{
	return name;
}
