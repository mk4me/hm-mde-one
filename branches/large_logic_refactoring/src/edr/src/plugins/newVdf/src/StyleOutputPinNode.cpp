#include "NewVdfPCH.h"
#include "StyleOutputPinNode.h"
#include "StyleItem.h"

using namespace vdf;

StyleOutputPinNode::StyleOutputPinNode() :
	item(new StyleItem()),
	index(-1)
{
    item->setFlag(QGraphicsItem::ItemIsMovable, false);
}

StyleOutputPinNode::~StyleOutputPinNode()
{

}


void StyleOutputPinNode::markIncomplete()
{
    throw std::exception("The method or operation is not implemented.");
}

void StyleOutputPinNode::markNormal()
{
	strategy->setState(IPinStrategy::Normal);
	item->update();
}

void StyleOutputPinNode::markCompatible()
{
    throw std::exception("The method or operation is not implemented.");
}

void StyleOutputPinNode::markIncompatible()
{
    throw std::exception("The method or operation is not implemented.");
}

void StyleOutputPinNode::markConnective()
{
	strategy->setState(IPinStrategy::Connective);
	item->update();
}

void StyleOutputPinNode::markUnconnective()
{
	strategy->setState(IPinStrategy::Unconnective);
	item->update();
}

void StyleOutputPinNode::markConnected()
{
    throw std::exception("The method or operation is not implemented.");
}

QGraphicsItem * StyleOutputPinNode::visualItem() const
{
    return item;
}

void StyleOutputPinNode::addSelection()
{
    throw std::exception("The method or operation is not implemented.");
}

void StyleOutputPinNode::removeSelection()
{
    throw std::exception("The method or operation is not implemented.");
}

void StyleOutputPinNode::addHover()
{
    throw std::exception("The method or operation is not implemented.");
}

void StyleOutputPinNode::removeHover()
{
    throw std::exception("The method or operation is not implemented.");
}

void StyleOutputPinNode::addCollision()
{
    throw std::exception("The method or operation is not implemented.");
}

void StyleOutputPinNode::removeCollision()
{
    throw std::exception("The method or operation is not implemented.");
}

void StyleOutputPinNode::setVisualStrategy( IVisualStrategyPtr strategy )
{
	this->strategy = core::dynamic_pointer_cast<IPinStrategy>(strategy);
	item->setStrategy(strategy);
}

void vdf::StyleOutputPinNode::removeConnection( IVisualConnectionWeakPtr connection )
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

void vdf::StyleOutputPinNode::addConnection( IVisualConnectionWeakPtr connection )
{
    connections.push_back(connection);
}
