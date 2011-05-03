#include <dfmlib/DFModel.h>
#include <dfmlib/DFSourceNode.h>
#include <dfmlib/Connection.h>
#include <algorithm>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

DFModel::DFModel(void) : Model(), DFInterface(false), pendingDisable(false), finishedLeafes(0)
{

}


DFModel::~DFModel(void)
{

}

void DFModel::onEnableChange()
{
	
}

void DFModel::addNode(NPtr node)
{
	if(validateOperation() == false || validateNodeType(node) == false){
		return;
	}

	Model::addNode(node);
	//update source nodes
	DFSNPtr sourceNode(boost::dynamic_pointer_cast<DFSourceNode>(node));
	if(sourceNode != nullptr){
		sourceNodes.insert(sourceNode);
	}else if(node->getInPins().empty() == false){
		//updates nodes requiring connections and their pins
		updateRequiredConnections(node);
	}
}

bool DFModel::sourcesHaveMoreData() const
{
	bool ret = true;

	for(auto it = sourceNodes.begin(); it != sourceNodes.end(); it++){
		if((*it)->hasMoreData() == false){
			ret = false;
			break;
		}
	}

	return ret;
}

void DFModel::updateRequiredConnections(NPtr node)
{
	Node::PINS_SET requiredPins;
	const Node::PINS_SET & pins = node->getInPins();
	for(auto it = pins.begin(); it != pins.end(); it++){
		if((*it)->isRequired() == true){
			requiredPins.insert(*it);
		}
	}

	pinsRequiringConnections[node].insert(requiredPins.begin(), requiredPins.end());
}

void DFModel::removeNode(NPtr node)
{
	if(validateOperation() == false){
		return;
	}

	//get connected nodes as inputs
	//buffer this data as further we have to disconnect the node, so we will loos this information
	NODES_SET inputNodes;
	const Node::PINS_SET & inputPins = node->getInPins();
	for(auto it = inputPins.begin(); it != inputPins.end(); it++){
		const CONNECTIONS_SET & connections = (*it)->getConnections();
		for(auto iT = connections.begin(); iT != connections.end(); iT++){
			inputNodes.insert((*iT)->getOther(*it)->getParent());
		}
	}

	//get connected pins as outputs
	//buffer this data as further we have to disconnect the node, so we will loos this information
	Node::PINS_SET outputPins;
	const Node::PINS_SET & outPins = node->getOutPins();
	for(auto it = inputPins.begin(); it != inputPins.end(); it++){
		const CONNECTIONS_SET & connections = (*it)->getConnections();
		for(auto iT = connections.begin(); iT != connections.end(); iT++){
			outputPins.insert((*iT)->getOther(*it));
		}
	}

	Model::removeNode(node);
	pinsRequiringConnections.erase(node);
	removeLeaf(node);

	//check src`s if have became leafs - no out connections
	//and add to lafs if required
	for(auto it = inputNodes.begin(); it != inputNodes.end(); it++){
		if(Node::anyOutPinConnected(*it) == false){
			addLeaf(*it);
		}
	}

	//update pins as requiring connections if we have left them with not connected inputs and they are required
	//automatically update nodes
	for(auto it = outputPins.begin(); it != outputPins.end(); it++){
		bool isNodeRequired = false;
		if(Node::anyInPinConnected((*it)->getParent()) == false){
			bool refresh = false;
			auto iT = pinsRequiringConnections.find((*it)->getParent());
			if(iT == pinsRequiringConnections.end()){
				pinsRequiringConnections[(*it)->getParent()];
				refresh = true;
			}

			PinPtr pin(*it);

			if(pin->isRequired() == true){
				if(refresh = true){
					iT = pinsRequiringConnections.find(pin->getParent());
				}

				iT->second.insert(pin);
			}
		}
	}
}

void DFModel::clearNodes()
{
	if(validateOperation() == false){
		return;
	}

	Model::clearNodes();
}

bool DFModel::canConnect(CPinPtr src, CPinPtr dest) const
{
	bool ret = true;

	if( Model::canConnect(src, dest) == false || src->getParent() == dest->getParent() ||
		dest->getConnections().empty() == false || createCycle(src, dest) == true){
		ret = false;
	}

	return ret;
}

ConnPtr DFModel::quickConnect(PinPtr src, PinPtr dest)
{
	ConnPtr ret(Model::quickConnect(src,dest));
	//check if dest parent has other connections
	//add it to leafes if no
	if(Node::anyOutPinConnected(dest->getParent()) == false){
		addLeaf(dest->getParent());
	}
	updateRequireConnections(dest);
	//src parent cannot be leaf anymore
	removeLeaf(src->getParent());

	return ret;
}

ConnPtr DFModel::connect(PinPtr src, PinPtr dest)
{
	if(validateOperation() == false || canConnect(src, dest) == false){
		return ConnPtr();
	}

	return quickConnect(src,dest);
}

void DFModel::updateRequireConnections(PinPtr pin)
{
	//verify if node already connected properly
	auto iT = pinsRequiringConnections.find(pin->getParent());
	if(iT != pinsRequiringConnections.end()){
		iT->second.erase(pin);
		if(iT->second.empty() == true){
			//it is already properly connected
			//so remove it
			pinsRequiringConnections.erase(iT);
		}
	}
}

bool DFModel::validateModel() const
{
	//minimum 2 elements - 1 source and one processing, no nodes requiring extra connections
	if(getNodes().empty() == true || sourceNodes.empty() == true || pinsRequiringConnections.empty() == false){
		return false;
	}	

	return true;
}


void DFModel::notifySources()
{
	for(auto it = sourceNodes.begin(); it != sourceNodes.end(); it++){
		(*it)->notify();
	}
}

bool DFModel::canRemoveNode(NPtr node)
{
	for(auto it = node->getOutPins().begin(); it != node->getOutPins().end(); it++){
		for(auto iT = (*it)->getConnections().begin(); iT != (*it)->getConnections().end(); iT++){
			if(canDisconnect(*iT) == false){
				return false;
			}
		}
	}

	return true;
}

bool DFModel::canDisconnect(ConnPtr connection)
{
	PinPtr dest = connection->getDest();
	NPtr destNode(dest->getParent());

	//check if disconnected input pin of destination node is required by some of its connected output pins
	const Node::PINS_SET & outPins = destNode->getOutPins();
	for(auto it = outPins.begin(); it != outPins.end(); it++){
		//we check if connections exist and if it is dependent on source pin
		if( (*it)->getConnections().empty() == false	&& (*it)->dependsOnPin(dest) == true){
			return false;
		}
	}

	return true;
}

void DFModel::initCheckedNodes(NODES_SET & nodes) const
{
    //nodes.insert(sourceNodes.begin(), sourceNodes.end());
    nodes.insert(leafNodes.begin(), leafNodes.end());
}

void DFModel::removeConnection(ConnPtr connection){
	if(validateOperation() == false || getConnections().find(connection) == getConnections().end()){
		return;
	}

	quickRemoveConnection(connection);
}

void DFModel::quickRemoveConnection(ConnPtr connection)
{
	Model::quickRemoveConnection(connection);

	//either required pin disconnected or no input connections at all!!
	if(Node::anyInPinConnected(connection->getDest()->getParent()) == false){
		pinsRequiringConnections[connection->getDest()->getParent()];
	}

	PinPtr pin(connection->getDest());

	if(pin->isRequired() == true){
		pinsRequiringConnections[pin->getParent()].insert(pin);
	}
}

void DFModel::clearConnections()
{
	const NODES_SET & nodes = getNodes();
    if(validateOperation() == false || nodes.size() == sourceNodes.size()){
		return;
	}

    //wyczysc problemy
    //pinsRequiringConnections.swap(REQUIRING_CONNECTION());

	Model::clearConnections();

	//set difference to insert all non source elements to require connections set
	std::vector<NPtr> difference(nodes.size() - sourceNodes.size());
	auto ending = std::set_difference(nodes.begin(), nodes.end(),
		sourceNodes.begin(), sourceNodes.end(), difference.begin());

	//update nodes requiring connection and their required pins
	for(auto it = difference.begin(); it != difference.end(); it++){
		updateRequiredConnections(*it);
	}

    //clear leaf nodes and their leaf pins
    for(auto it = leafFakePins.begin(); it != leafFakePins.end(); it++){
        unregisterPin(it->second);
    }

    leafFakePins.swap(std::map<NPtr, PinPtr>());
    leafNodes.swap(NODES_SET());
}

bool DFModel::validateOperation() const
{
	if(isEnable() == true){
		return false;
	}

	return true;
}

bool DFModel::validateNodeType(NPtr node)
{
	if(DFNode::getDFNode(node) == nullptr){
		return false;
	}

	return true;
}

DFModel::FakePin::FakePin(WDFMPtr model) : DFPin(), model(model)
{

}

void DFModel::FakePin::onUpdate()
{
	if(model.lock() != nullptr){
		model.lock()->leafHasProcessedData();
	}
}

bool DFModel::FakePin::isCompatible(PinPtr pin) const
{
	return false;
}

void DFModel::leafHasProcessedData()
{
	if(++finishedLeafes == leafFakePins.size()){
		if(pendingDisable == true){
			pendingDisable = false;
			setEnable(false);
		}else{
			finishedLeafes = 0;
			if(sourcesHaveMoreData() == true){
				resetPinStates();
				notifySources();
			}else{
				setEnable(false);
			}
		}
	}
}

void DFModel::process()
{
    if(isEnable() == true){
        if(validateModel() == true && sourcesHaveMoreData() == true){
            resetPinStates();
            resetNodeStates();
            notifySources();
        }
    }else{
        pendingDisable = true;
    }
}

void DFModel::resetPinStates()
{
	for(auto it = getNodes().begin(); it != getNodes().end(); it++){
		DFNPtr node = boost::dynamic_pointer_cast<DFNode>(*it);
		if(node != 0){
			for(auto iT = node->getInPins().begin(); iT != node->getInPins().end(); iT++){
				resetPinState(*iT);
			}

			for(auto iT = node->getOutPins().begin(); iT != node->getOutPins().end(); iT++){
				resetPinState(*iT);
			}
		}
	}
}

void DFModel::resetPinState(PinPtr pin)
{
	DFPinPtr p(DFPin::getDFPin(pin));
	//only DFPins are supported
	if(p != nullptr){
		p->reset();
	}
}

void DFModel::resetNodeStates()
{
	for(auto it = getNodes().begin(); it != getNodes().end(); it++){
		DFNPtr node = DFNode::getDFNode(*it);
		if(node != nullptr){
			node->reset();
		}
	}
}

void DFModel::addLeaf(NPtr node)
{
	//check if node is not a leafe already
	if(leafFakePins.find(node) == leafFakePins.end()){
		PinPtr pin(new FakePin(WDFMPtr(boost::dynamic_pointer_cast<DFModel>(shared_from_this()))));
		node->addOutPin(pin);
		leafFakePins[node] = pin;
		leafNodes.insert(node);
	}
}

void DFModel::removeLeaf(NPtr node)
{
	auto it = leafFakePins.find(node);
	if(it != leafFakePins.end()){
		unregisterPin(it->second);
		leafFakePins.erase(node);
		leafNodes.erase(node);
	}
}

const DFModel::SOURCE_NODES_SET & DFModel::getSourceNodes() const
{
	return sourceNodes;
}

const Model::NODES_SET & DFModel::getLeafNodes() const
{
	return leafNodes;
}

const DFModel::REQUIRING_CONNECTION & DFModel::getRequiringConnections() const
{
	return pinsRequiringConnections;
}

}