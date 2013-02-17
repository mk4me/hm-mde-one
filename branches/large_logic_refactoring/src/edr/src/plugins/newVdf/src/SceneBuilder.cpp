#include "NewVdfPCH.h"
#include "SceneBuilder.h"
#include <plugins/newVdf/IVisualItem.h>
#include <plugins/newVdf/IVisualProcessingNode.h>
#include <plugins/newVdf/IVisualSinkNode.h>
#include <plugins/newVdf/IVisualSourceNode.h>
#include <plugins/newVdf/IVisualInputPin.h>
#include <plugins/newVdf/IVisualOutputPin.h>

SceneBuilder::VisualNodeWithPins SceneBuilder::createProcessing(const QString& text, df::INode* prototype, int in, int out ) const
{
    IVisualProcessingNodePtr node = factories->getCurrentNodesFactory()->createProcessingNode();
    node->setName(text);
	node->setModelNode(prototype);
    auto input = addInputPins(node, in);
    auto output = addOutputPins(node, out);
	node->visualItem()->setZValue(0.0f);
    return boost::make_tuple(node, input, output);
}

SceneBuilder::VisualNodeWithPins SceneBuilder::createSink(const QString& text, df::INode* prototype, int in ) const
{
    IVisualSinkNodePtr node = factories->getCurrentNodesFactory()->createSinkNode();
    node->setName(text);
	node->setModelNode(prototype);
    auto input = addInputPins(node, in);
	node->visualItem()->setZValue(0.0f);
    return boost::make_tuple(node, input, Pins());
}

SceneBuilder::VisualNodeWithPins SceneBuilder::createSource(const QString& text, df::INode* prototype, int out ) const
{
    IVisualSourceNodePtr node = factories->getCurrentNodesFactory()->createSourceNode();
    node->setName(text);
	node->setModelNode(prototype);
    auto output = addOutputPins(node, out);
	node->visualItem()->setZValue(0.0f);
    return boost::make_tuple(node, Pins(), output);
}

SceneBuilder::Pins SceneBuilder::addInputPins( IVisualSinkNodePtr sink, int count ) const
{
    Pins pins;
    for (int i = 0; i < count; ++i) {
        IVisualInputPinPtr pin = factories->getCurrentPinsFactory()->createInputPin();
		pin->visualItem()->setZValue(1.0f);
		pin->setParent(sink);
		pin->setIndex(i);
        sink->addInputPin(pin);
        pins.push_back(pin);
    }
    return std::move(pins);
}

SceneBuilder::Pins SceneBuilder::addOutputPins( IVisualSourceNodePtr source, int count ) const
{
    Pins pins;
    for (int i = 0; i < count; ++i) {
        IVisualOutputPinPtr pin = factories->getCurrentPinsFactory()->createOutputPin();
		pin->visualItem()->setZValue(1.0f);
		pin->setParent(source);
		pin->setIndex(i);
        source->addOutputPin(pin);
        pins.push_back(pin);
    }
    return std::move(pins);
}

SceneBuilder::VisualNodeWithPins  SceneBuilder::createType(const QString& text, df::INode* prototype, int in, int out) const
{
    if (in > 0 && out > 0) {
        return createProcessing(text, prototype, in, out);
    } else if (out > 0) {
        return createSource(text, prototype, out);
    } else if (in > 0) {
        return createSink(text, prototype, in);
    }

    UTILS_ASSERT(false);
    return boost::make_tuple((IVisualNode*)nullptr, Pins(), Pins());
}

SceneBuilder::SceneBuilder( CanvasStyleEditorPtr factories ) : 
     factories(factories)
{

}

IVisualConnectionPtr SceneBuilder::createConnection( IVisualPinPtr pin1, IVisualPinPtr pin2 )
{
    IVisualConnectionPtr connection = factories->getCurrentConnectionsFactory()->createConnection();
    connection->setBegin(pin1);
    connection->setEnd(pin2);
	connection->visualItem()->setZValue(0.5f);
	connection->visualItem()->setFlag(QGraphicsItem::ItemIsMovable, false);
	connection->visualItem()->setFlag(QGraphicsItem::ItemIsSelectable,false);
	pin1->setConnection(connection);
	pin2->setConnection(connection);
    return connection;
}
