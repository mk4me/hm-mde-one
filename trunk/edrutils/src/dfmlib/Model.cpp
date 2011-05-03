#include <dfmlib/Model.h>
#include <dfmlib/Pin.h>
#include <dfmlib/Connection.h>
#include <utils/Debug.h>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

Model::Model(void)
{
}

Model::~Model(void)
{
}

void Model::addNode( NPtr node )
{
	UTILS_ASSERT((node != nullptr), "Bledny wezel");
    
    if(nodes.find(node) != nodes.end()){
        throw std::runtime_error("Node already added to model!");
    }

    nodes.insert(node);
}

void Model::removeNode( NPtr node )
{
    UTILS_ASSERT((node != nullptr), "Bledny wezel");
	if(nodes.erase(node) == 0){
		throw std::runtime_error("Node does not exist in model!");
	}

    disconnectNode(node);
}

void Model::clearNodes(){
    clearConnections();
	connections.swap(CONNECTIONS_SET());
	nodes.swap(NODES_SET());
}

bool Model::canConnect(CPinPtr src, CPinPtr dest) const {
	bool ret = true;

	if(src->getType() != Pin::PIN_OUT || dest->getType() != Pin::PIN_IN || dest->isCompatible(src) == false
		|| findConnection(src, dest) != nullptr){
			ret = false;
	}

	return ret;
}

ConnPtr Model::quickConnect(PinPtr src, PinPtr dest){
	ConnPtr ret(new Connection(src, dest));
	connections.insert(ret);
	src->addConnection(ret);
	dest->addConnection(ret);
	return ret;
}

ConnPtr Model::connect(PinPtr src, PinPtr dest){
	ConnPtr ret;
	if(canConnect(src, dest) == true){
		ret = quickConnect(src,dest);
	}

	return ret;
}

void Model::removeConnection( ConnPtr connection )
{
	if(connections.find(connection) == connections.end()){
		throw std::runtime_error("Connection dos not exist in model!");
	}

    quickRemoveConnection(connection);
}

Model::PATH_ENTRY Model::getFirstNodeOutputConnection(NPtr node){
	PATH_ENTRY pathElement;

	for(auto it = node->getOutPins().begin(); it != node->getOutPins().end(); it++){
		for(auto iT = (*it)->getConnections().begin(); iT != (*it)->getConnections().end(); iT++){
			pathElement.node = node;
			pathElement.pinIT = it;
			pathElement.connIT = iT;
			return pathElement;
		}
	}

	return pathElement;
}

Model::PATH_ENTRY Model::getNextNodeOutputConnection(const PATH_ENTRY & pathElement){
	PATH_ENTRY pe = pathElement;
	
	if(++pe.connIT != (*pe.pinIT)->getConnections().end()){
		return pe;
	}

	++pe.pinIT;

	for( ; pe.pinIT != pe.node->getOutPins().end(); pe.pinIT++){
		if((*pe.pinIT)->getConnections().empty() == false){
			pe.connIT = (*pe.pinIT)->getConnections().begin();
			return pe;
		}
	}

	return pathElement;
}

bool Model::PATH_ENTRY::operator==(const MY_PATH_ENTRY & pe) const{
	if(this->node == pe.node && this->pinIT == pe.pinIT && this->connIT == pe.connIT){
		return true;	
	}

	return false;
}

bool Model::createCycle(CPinPtr src, CPinPtr dest) const{
	return !getCycle(src, dest).empty();
}

void Model::initCheckedNodes(Model::NODES_SET & nodes) const
{

}

Model::CYCLE Model::getCycle(CPinPtr src, CPinPtr dest) const{
	//if source is connected no loops possible
	//also if leaf node is connected

	//connections in path
	CYCLE connInPath;

	NPtr destNode = dest->getParent();
	if(Node::anyOutPinConnected(destNode) == false){
		return connInPath;
	}

	PATH_ENTRY pe = getFirstNodeOutputConnection(destNode);

	//stores node compleately checked - up to leafs
	NODES_SET checkedNodes;

    initCheckedNodes(checkedNodes);

	//nodes in path
	NODES_SET nodesInPath;

	nodesInPath.insert(src->getParent());
	nodesInPath.insert(destNode);

	//structure for traversing
	std::vector<PATH_ENTRY> path;
	
	//find first output connection
	path.push_back(pe);
	//add it co connections
	connInPath.push_back(*path.back().connIT);

	//check cycles in structure starting from this connection
	while(path.empty() == false){
		NPtr nextNode = connInPath.back()->getDest()->getParent();

		//check for a cycle
		if(nodesInPath.find(nextNode) != nodesInPath.end()){
			//we have found a cycle
			break;
		}

		if(Node::anyOutPinConnected(nextNode) == false){
			checkedNodes.insert(nextNode);
			connInPath.pop_back();
		}

		//check if node fully verified
		if(checkedNodes.find(nextNode) != checkedNodes.end()){
			//if yes we can skip it, move back and continue procedure
			while(path.empty() == false && (pe = getNextNodeOutputConnection(path.back())) == path.back()){
				//move back
				checkedNodes.insert(path.back().node);
				nodesInPath.erase(path.back().node);
				if(connInPath.empty() == false){
					connInPath.pop_back();
				}
				path.pop_back();
			}

			if(path.empty() == false){
				path.back() = pe;
				connInPath.push_back(*pe.connIT);
			}

			continue;
		}

		//go to the next node!!
		//update nodes in path
		nodesInPath.insert(nextNode);

		path.push_back(getFirstNodeOutputConnection(nextNode));
		connInPath.push_back(*path.back().connIT);
	}

	return connInPath;
}

void Model::quickRemoveConnection( ConnPtr connection )
{
	connection->getSrc()->removeConnection(connection);
	connection->getDest()->removeConnection(connection);
	connections.erase(connection);
}

void Model::clearConnections(){
	while(connections.empty() == false){
		quickRemoveConnection(*connections.begin());
	}
}

const Model::NODES_SET & Model::getNodes() const{
	return nodes;
}

const Model::CONNECTIONS_SET & Model::getConnections() const{
	return connections;
}

void Model::disconnectNode( NPtr node )
{
	//get all node connections from all pins and remove them
	CONNECTIONS_SET allNodeConnections;

	for(auto it = node->getInPins().begin(); it != node->getInPins().end(); it++){
		const CONNECTIONS_SET & connections = (*it)->getConnections();
		allNodeConnections.insert(connections.begin(), connections.end());
	}

	for(auto it = node->getOutPins().begin(); it != node->getOutPins().end(); it++){
		const CONNECTIONS_SET & connections = (*it)->getConnections();
		allNodeConnections.insert(connections.begin(), connections.end());
	}

	//remove all node connections
	while(allNodeConnections.empty() == false){
		removeConnection(*allNodeConnections.begin());
		allNodeConnections.erase(allNodeConnections.begin());
	}
}

ConnPtr Model::findConnection(CPinPtr src, CPinPtr dest) const
{
	ConnPtr ret;
    if(nodes.find(src->getParent()) == nodes.end() || nodes.find(dest->getParent()) == nodes.end()){
		return ret;
	}

	const CONNECTIONS_SET * connections = nullptr;

	if(src->getConnections().size() > dest->getConnections().size()){
		connections = &(dest->getConnections());
		src.swap(dest);
	}else{
		connections = &(src->getConnections());
	}

	for(auto it = connections->begin(); it != connections->end(); it++){
		//connection exist and is registered by the model
		if((*it)->getOther(src) == dest && connections->find(*it) != connections->end() && connections->find(*it) != connections->end()){
			ret = *it;
			break;
		}
	}

	return ret;
}

void Model::unregisterPin(PinPtr pin){
	NPtr parent = pin->getParent();
	if(pin->getType() == Pin::PIN_IN){
		parent->inPins.erase(parent->inPins.find(pin));
	}else if(pin->getType() == Pin::PIN_OUT){
		parent->outPins.erase(parent->outPins.find(pin));
	}
}

}