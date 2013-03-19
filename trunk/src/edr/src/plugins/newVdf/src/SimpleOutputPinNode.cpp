#include "NewVdfPCH.h"
#include "SimpleOutputPinNode.h"
#include "SimpleItem.h"
#include <dflib/INode.h>

using namespace vdf;

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

void vdf::SimpleOutputPinNode::removeConnection( core::weak_ptr<IVisualConnection> connection )
{
    auto c = connection.lock();
    auto it = std::find_if(connections.begin(), connections.end(), 
        [&](const IVisualConnectionWeakPtr& val)
        { 
            return val.lock() == c; 
        }
    );
    if (it != connections.end()) {
        connections.erase(it);
    }
}

df::IOutputPin* vdf::SimpleOutputPinNode::getModelPin() const
{
    IVisualNodePtr parent = getParent().lock();
    UTILS_ASSERT(parent);

    df::INode* modelNode = parent->getModelNode();
    UTILS_ASSERT(modelNode && index >= 0);

    df::ISourceNode* source = dynamic_cast<df::ISourceNode*>(modelNode);
    return source->outputPin(index);
}

QPointF vdf::SimpleOutputPinNode::getConnectionPosHint()
{
    return visualItem()->scenePos();
}
