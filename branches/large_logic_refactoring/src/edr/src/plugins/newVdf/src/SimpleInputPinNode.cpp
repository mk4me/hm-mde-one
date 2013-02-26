#include "NewVdfPCH.h"
#include "SimpleInputPinNode.h"
#include "SimpleItem.h"

using namespace vdf;

SimpleInputPinNode::SimpleInputPinNode() :
    item(new SimplePinItem()),
	index(-1)
{

}

SimpleInputPinNode::~SimpleInputPinNode()
{

}


void SimpleInputPinNode::markRequired()
{
    throw std::exception("The method or operation is not implemented.");
}

void SimpleInputPinNode::markMissing()
{
    throw std::exception("The method or operation is not implemented.");
}

void SimpleInputPinNode::markNormal()
{
    item->setColor(QColor(180, 180, 180));
	item->update();
}

void SimpleInputPinNode::markCompatible()
{
    throw std::exception("The method or operation is not implemented.");
}

void SimpleInputPinNode::markIncompatible()
{
    throw std::exception("The method or operation is not implemented.");
}

void SimpleInputPinNode::markConnective()
{
   item->setColor(QColor(50, 50, 200));
   item->update();
}

void SimpleInputPinNode::markUnconnective()
{
    item->setColor(QColor(230, 0, 0));
	item->update();
}

void SimpleInputPinNode::markConnected()
{
    throw std::exception("The method or operation is not implemented.");
}

QGraphicsItem * SimpleInputPinNode::visualItem() const
{
    return item;
}

void SimpleInputPinNode::addSelection()
{
    throw std::exception("The method or operation is not implemented.");
}

void SimpleInputPinNode::removeSelection()
{
    throw std::exception("The method or operation is not implemented.");
}

void SimpleInputPinNode::addHover()
{
    throw std::exception("The method or operation is not implemented.");
}

void SimpleInputPinNode::removeHover()
{
    throw std::exception("The method or operation is not implemented.");
}

void SimpleInputPinNode::addCollision()
{
    throw std::exception("The method or operation is not implemented.");
}

void SimpleInputPinNode::removeCollision()
{
    throw std::exception("The method or operation is not implemented.");
}

void SimpleInputPinNode::setVisualStrategy( IVisualStrategyPtr strategy )
{
	throw std::exception("The method or operation is not implemented.");
}




df::IInputPin* vdf::SimpleInputPinNode::getModelPin() const
{
    IVisualNodePtr parent = getParent().lock();
    UTILS_ASSERT(parent);

    df::INode* modelNode = parent->getModelNode();
    UTILS_ASSERT(modelNode && index >= 0);

    df::ISinkNode* sink = dynamic_cast<df::ISinkNode*>(modelNode);
    return sink->inputPin(index);
}
