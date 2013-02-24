#include "NewVdfPCH.h"
#include "SceneModel.h"
#include <plugins/newVdf/IVisualItem.h>
#include <plugins/newVdf/IVisualOutputPin.h>
#include <plugins/newVdf/IVisualInputPin.h>
#include <plugins/newVdf/IVisualSinkNode.h>
#include <plugins/newVdf/IVisualSourceNode.h>
#include <plugins/newVdf/IVisualProcessingNode.h>
#include <dflib/Model.h>
#include <dflib/DFModelRunner.h>
#include <dflib/Connection.h>
#include <dflib/Node.h>
#include <type_traits>

using namespace vdf;

IVisualConnectionPtr SceneModel::addConnection(IVisualPinPtr pin1, IVisualPinPtr pin2)
{
   	UTILS_ASSERT(pin1 && pin2);

	IVisualNodePtr parent1 = pin1->getParent().lock();
	IVisualNodePtr parent2 = pin2->getParent().lock();

	df::INode* modelNode1 = parent1->getModelNode();
	df::INode* modelNode2 = parent2->getModelNode();

	
	df::IConnection* con;
	if (pin1->isType(IVisualItem::InputPin) && pin2->isType(IVisualItem::OutputPin)) {
		auto inputPin = getInputPin(modelNode1, pin1->getIndex());
		auto outputPin = getOutputPin(modelNode2, pin2->getIndex());
		con = new df::Connection(outputPin, inputPin);
	} else if (pin1->isType(IVisualItem::OutputPin) && pin2->isType(IVisualItem::InputPin)) {
		auto inputPin = getInputPin(modelNode2, pin2->getIndex());
		auto outputPin = getOutputPin(modelNode1, pin1->getIndex());
		con = new df::Connection(outputPin, inputPin);
	} else {
		UTILS_ASSERT(false);
	}

	model->addConnection(con);


    IVisualConnectionPtr connection = builder.createConnection(pin1, pin2);
	connection->setModelConnection(con);
    addItem(connection);
    return connection;
}


SceneModel::SceneModel( CanvasStyleEditorPtr factories ) :
    builder(factories),
	model(new df::Model)
{

}

void SceneModel::addItem( IVisualItemPtr item )
{
    auto* graphics = item->visualItem();
    graphics2Visual[graphics] = item;
    // TODO : czy to zapewni, ze nie zostana dodane dwa te same elementy?
    if (graphics2Visual.find(graphics->parentItem()) == graphics2Visual.end()) {
        emit visualItemAdded(item);
	}

	if (item->isType(IVisualItem::Node)) {
		IVisualNodePtr node = core::dynamic_pointer_cast<IVisualNode>(item);
		addNode(node->getModelNode());
	}
	if (item->isType(IVisualItem::InputPin)) {
		inputPins.push_back(core::dynamic_pointer_cast<IVisualInputPin>(item)); 
	} else if (item->isType(IVisualItem::OutputPin)) {
		outputPins.push_back(core::dynamic_pointer_cast<IVisualOutputPin>(item));
	}
}

bool SceneModel::connectionPossible( IVisualPinPtr pin1, IVisualPinPtr pin2 ) const
{
	// todo : rozwinac
	if (pin1 == pin2 || pin1->getType() == pin2->getType()) {
		return false;
	} 
	return true;
}

//bool SceneModel::connectionPossible( QGraphicsItem* pin1, QGraphicsItem* pin2 ) const
//{
//	auto it = graphics2Visual.find(pin1);
//	IVisualPin* vpin1 = it != graphics2Visual.end() ? dynamic_cast<IVisualPin*>(it->second) : nullptr;
//	it = graphics2Visual.find(pin2);
//	IVisualPin* vpin2 = it != graphics2Visual.end() ? dynamic_cast<IVisualPin*>(it->second) : nullptr;
//	if(vpin1 && vpin2) {
//		return connectionPossible(vpin1, vpin2);
//	} 
//	return false;
//}

void SceneModel::addNode(df::INode* node )
{
	UTILS_ASSERT(node);

	switch(node->type()) {
		case df::INode::PROCESSING_NODE: {
			df::IProcessingNode* processing = dynamic_cast<df::IProcessingNode*>(node);
			model->addNode(processing); 
		} return;

		case df::INode::SINK_NODE: {
			df::ISinkNode* sink = dynamic_cast<df::ISinkNode*>(node);
			model->addNode(sink);
		} return;

		case df::INode::SOURCE_NODE: {
			df::ISourceNode* source = dynamic_cast<df::ISourceNode*>(node);
			model->addNode(source);
		} return;
	}
	
	UTILS_ASSERT(false);
}

void SceneModel::run()
{
	df::DFModelRunner runner;
	bool test = df::DFModelRunner::verifyModel(model.get());
	runner.start(model.get(), nullptr);
	runner.join();
}

df::IInputPin* SceneModel::getInputPin( df::INode* node, int index )
{
	UTILS_ASSERT(node && index >= 0);

	switch(node->type()) {
		case df::INode::PROCESSING_NODE: {
			df::IProcessingNode* processing = dynamic_cast<df::IProcessingNode*>(node);
			return processing->inputPin(index);
		}

		case df::INode::SINK_NODE: {
			df::ISinkNode* sink = dynamic_cast<df::ISinkNode*>(node);
			return sink->inputPin(index);
		}
	}

	UTILS_ASSERT(false);
	return nullptr;
}

df::IOutputPin* SceneModel::getOutputPin( df::INode* node, int index )
{
	UTILS_ASSERT(node && index >= 0);

	switch(node->type()) {
		case df::INode::PROCESSING_NODE: {
			df::IProcessingNode* processing = dynamic_cast<df::IProcessingNode*>(node);
			return processing->outputPin(index);
		}

		case df::INode::SOURCE_NODE: {
			df::ISourceNode* source = dynamic_cast<df::ISourceNode*>(node);
			return source->outputPin(index);
		}
	}

	UTILS_ASSERT(false);
	return nullptr;
}

const SceneModel::Connections& SceneModel::getPossibleConections(IVisualPinPtr vpin  ) 
{
	pinsHelper.impossible.clear();
	pinsHelper.possible.clear();
	// todo : optymalizacja
	for (auto it = inputPins.begin(); it != inputPins.end(); ++it) {
		if (*it != vpin) {
			if (connectionPossible(vpin, *it)) {
				pinsHelper.possible.push_back(*it);
			} else {
				pinsHelper.impossible.push_back(*it);
			}
		}
	}
	for (auto it = outputPins.begin(); it != outputPins.end(); ++it) {
		if (*it != vpin) {
			if (connectionPossible(vpin, *it)) {
				pinsHelper.possible.push_back(*it);
			} else {
				pinsHelper.impossible.push_back(*it);
			}
		}
	}
	return pinsHelper;
}

template<class VisualPinT>
void vdf::SceneModel::removePin( IVisualItemPtr item )
{
	auto pin = core::dynamic_pointer_cast<VisualPinT>(item);
	UTILS_ASSERT(pin);
	auto connection = pin->getConnection().lock();
	if (connection) {
		removeItem(connection);
	}

	if (std::is_same<VisualPinT, IVisualInputPin>::value) {
		inputPins.remove(core::dynamic_pointer_cast<IVisualInputPin>(pin));
	} else if (std::is_same<VisualPinT, IVisualOutputPin>::value) {
		outputPins.remove(core::dynamic_pointer_cast<IVisualOutputPin>(pin));
	} else {
		UTILS_ASSERT(false);
	}
}

template<class VisualT, class DFNodeT>
void SceneModel::removeNode( IVisualItemPtr item )
{
	auto node = core::dynamic_pointer_cast<VisualT>(item);
	UTILS_ASSERT(node);
	auto* ptr = dynamic_cast<DFNodeT*>(node->getModelNode());
	UTILS_ASSERT(ptr);
	model->removeNode(ptr);

	if (std::is_same<VisualT, IVisualSourceNode>::value || std::is_base_of<IVisualSourceNode, VisualT>::value) {
		removeOutputPins(node);
	}

	if (std::is_same<VisualT, IVisualSinkNode>::value || std::is_base_of<IVisualSinkNode, VisualT>::value) {
		removeInputPins(node);
	}
}


void SceneModel::removeItem( IVisualItemPtr item )
{
	auto* graphics = item->visualItem();
	auto it = graphics2Visual.find(graphics);
	UTILS_ASSERT(it != graphics2Visual.end());
	graphics2Visual.erase(it);
	emit visualItemRemoved(item);

	switch(item->getType()) {
	case IVisualItem::ProcessingNode:
		removeNode<IVisualProcessingNode, df::ProcessingNode>(item);
		break;

	case IVisualItem::SinkNode:
		removeNode<IVisualSinkNode, df::SinkNode>(item);
		break;

	case IVisualItem::SourceNode:
		removeNode<IVisualSourceNode, df::SourceNode>(item);
		break;

	case (IVisualItem::InputPin):
		removePin<IVisualInputPin>(item);
		break;

	case (IVisualItem::OutputPin):
		removePin<IVisualOutputPin>(item);
		break;

	case (IVisualItem::Connection):
		IVisualConnectionPtr connection = core::dynamic_pointer_cast<IVisualConnection>(item);
		connection->getBegin()->setConnection(IVisualConnectionWeakPtr());
		connection->getEnd()->setConnection(IVisualConnectionWeakPtr());
		try {
			model->removeConnection(connection->getModelConnection());
		} catch (std::exception& e) {

		}
		break;
	}
}

IVisualItemPtr SceneModel::tryGetVisualItem( QGraphicsItem* item )
{
	auto it = graphics2Visual.find(item);
	if (it != graphics2Visual.end()) {
		return it->second;
	}

	return IVisualItemPtr();
}

void SceneModel::removeConnection( IVisualPinPtr pin1, IVisualPinPtr pin2 )
{
	UTILS_ASSERT(pin1->getConnection().lock() == pin2->getConnection().lock());
	auto connection = pin1->getConnection().lock();
	removeItem(connection);
}

void SceneModel::merge( const QList<QGraphicsItem*>& items )
{
	QList<IVisualNodePtr> nodes = this->getVisualItems<IVisualNodePtr>(items);
	MergedItemPtr merged(new MergedItem());
	merged->nodes = nodes;

	std::vector<IVisualInputPinPtr> ipins;
	std::vector<IVisualOutputPinPtr> opins;

	for (auto it = nodes.begin(); it != nodes.end(); ++it) {
		auto graphicsItem = (*it)->visualItem();
		graphicsItem->setVisible(false);
		graphicsItem->update();
		IVisualSourceNodePtr source = core::dynamic_pointer_cast<IVisualSourceNode>(*it);
		if (source) {
			int count = source->getNumOutputPins();
			for (int i = 0; i < count; ++i) {
				IVisualOutputPinPtr opin = source->getOutputPin(i);
				auto connection = opin->getConnection().lock();
				if (connection) {
					connection->visualItem()->setVisible(false);
				}
				opins.push_back(opin);
			}
		}
		IVisualSinkNodePtr sink = core::dynamic_pointer_cast<IVisualSinkNode>(*it);
		if (sink) {
			int count = sink->getNumInputPins();
			for (int i = 0; i < count; ++i) {
				IVisualInputPinPtr ipin = sink->getInputPin(i);
				auto connection = ipin->getConnection().lock();
				if (connection) {
					connection->visualItem()->setVisible(false);
				} else {
					ipins.push_back(sink->getInputPin(i));
				}
			}
		}
	}
	if (!ipins.empty() && !opins.empty()) {
		IVisualProcessingNodePtr node = builder.factories->getCurrentNodesFactory()->createProcessingNode();
		node->setName("Merged");
		for (auto it = ipins.begin(); it != ipins.end(); ++it) {
			node->addInputPin(*it);
		}
		for (auto it = opins.begin(); it != opins.end(); ++it) {
			node->addOutputPin(*it);
		}
		merged->node = node;
						
	}
	emit visualItemAdded(merged->node);
	
}


void vdf::SceneModel::clearScene()
{
    auto nodes = getVisualItems<IVisualNodePtr>();
	for (auto it = nodes.begin(); it != nodes.end(); ++it) {
        removeItem(*it);
    }
    UTILS_ASSERT(graphics2Visual.empty());
    UTILS_ASSERT(inputPins.empty());
    UTILS_ASSERT(outputPins.empty());
}

void vdf::SceneModel::removeInputPins(IVisualNodePtr node )
{
	IVisualSinkNodePtr sink = core::dynamic_pointer_cast<vdf::IVisualSinkNode>(node);
	if (sink) {
		int count = sink->getNumInputPins();
		for (int i = 0; i < count; ++i) {
			removeItem(sink->getInputPin(i));
		}
	}
}

void vdf::SceneModel::removeOutputPins(IVisualNodePtr node) 
{
	IVisualSourceNodePtr source = core::dynamic_pointer_cast<vdf::IVisualSourceNode>(node);
	if (source) {
		int count = source->getNumOutputPins();
		for (int i = 0; i < count; ++i) {
			removeItem(source->getOutputPin(i));
		}
	}
}
