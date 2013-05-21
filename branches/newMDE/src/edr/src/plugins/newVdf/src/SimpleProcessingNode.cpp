#include "NewVdfPCH.h"
#include "SimpleProcessingNode.h"
#include <plugins/newVdf/IVisualOutputPin.h>
#include <plugins/newVdf/IVisualInputPin.h>
#include <plugins/newVdf/INodeStrategy.h>
#include "SimpleItem.h"

using namespace vdf;

SimpleProcessingNode::SimpleProcessingNode() :
    simpleItem(new SimpleTextItem(0,0))
{
}

void SimpleProcessingNode::addInputPin( IVisualInputPinPtr pin )
{
    pin->visualItem()->setPos(margin, margin + inputPins.size() * pinHeight);
    pin->visualItem()->setParentItem(simpleItem);
    inputPins.push_back(pin);
    simpleItem->setSize(calculateNodeSize(inputPins.size(), outputPins.size()));
}

void SimpleProcessingNode::removeInputPin( IVisualInputPinPtr pin )
{
    throw std::exception("The method or operation is not implemented.");
}

void SimpleProcessingNode::clearInputPins()
{
    throw std::exception("The method or operation is not implemented.");
}

void SimpleProcessingNode::setName( const QString & name )
{
    simpleItem->setText(name);
	this->name = name;
}

QString SimpleProcessingNode::getName() const 
{
	return name;
}

void SimpleProcessingNode::setConfigButton( QAbstractButton * button )
{
    throw std::exception("The method or operation is not implemented.");
}

void SimpleProcessingNode::setCloseButton( QAbstractButton * button )
{
    throw std::exception("The method or operation is not implemented.");
}

QGraphicsItem * SimpleProcessingNode::visualItem() const
{
    return simpleItem;
}

void SimpleProcessingNode::addSelection()
{
    simpleItem->setSelected(true);
}

void SimpleProcessingNode::removeSelection()
{
	simpleItem->setSelected(false);
}

void SimpleProcessingNode::addHover()
{
    throw std::exception("The method or operation is not implemented.");
}

void SimpleProcessingNode::removeHover()
{
    throw std::exception("The method or operation is not implemented.");
}

void SimpleProcessingNode::addCollision()
{
    throw std::exception("The method or operation is not implemented.");
}

void SimpleProcessingNode::removeCollision()
{
    throw std::exception("The method or operation is not implemented.");
}

void SimpleProcessingNode::addOutputPin( IVisualOutputPinPtr pin )
{
    pin->visualItem()->setParentItem(simpleItem);
    pin->visualItem()->setPos(margin + nodeWidth - pin2, margin + outputPins.size() * pinHeight);

    outputPins.push_back(pin);
    simpleItem->setSize(calculateNodeSize(inputPins.size(), outputPins.size()));
}

void SimpleProcessingNode::removeOutputPin( IVisualOutputPinPtr pin )
{
    throw std::exception("The method or operation is not implemented.");
}

void SimpleProcessingNode::clearOutputPins()
{
    throw std::exception("The method or operation is not implemented.");
}

