#include <dfmlib/Model.h>
#include <dfmlib/Pin.h>
#include <dfmlib/Connection.h>
#include <utils/Debug.h>
#include <OpenThreads/Thread>

#define NO_LOCK_THREADID -1

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

Model::Model(void)
{
}

Model::~Model(void)
{
}

bool Model::isModelChangeAllowed() const
{
    return true;
}

void Model::setNodeName(const NPtr & node, const std::string & name)
{
    UTILS_ASSERT((node != nullptr), "B³êdny wêze³ do edycji!");

    ScopedLock lock(editMutex);

    if(node->model.lock() != shared_from_this()){
        throw std::runtime_error("Node belongs to other model!");
    }

    node->setName(name);

    notify();
}

//! \param pin Pin któremu nazwê zmieniamy
//! \param name Nowa nazwa pinu
void Model::setPinName(const PinPtr & pin, const std::string & name)
{
    UTILS_ASSERT((pin != nullptr), "B³êdny pin do edycji!");

    ScopedLock lock(editMutex);

    if(pin->getParent()->model.lock().get() != this){
        throw std::runtime_error("Pin belongs to different model!");
    }

    pin->setName(name);

    notify();
}

const Model::Nodes & Model::getNodes() const
{
    ScopedLock lock(editMutex);
    return nodes;
}

const Model::Connections & Model::getConnections() const
{
    ScopedLock lock(editMutex);
    return connections;
}

const Model::RequiringConnection & Model::getRequiringConnections() const
{
    ScopedLock lock(editMutex);
    return pinsRequiringConnections;
}

const Model::Nodes & Model::getLeafNodes() const
{
    ScopedLock lock(editMutex);
    return leafNodes;
}

bool Model::isNodeSupported(const NPtr & node) const
{
    return true;
}

bool Model::additionalModelValidation() const
{
    return true;
}

bool Model::isModelValid() const
{
    ScopedLock lock(editMutex);
    return getRequiringConnections().empty() == true && additionalModelValidation() == true;
}

void Model::addNode(const NPtr & node)
{
	UTILS_ASSERT((node != nullptr), "Bledny wezel");

    if(node == nullptr){
        throw std::runtime_error("Invalid node!");
    }

    ScopedLock lock(editMutex);

    if(isModelChangeAllowed() == false){
        throw std::runtime_error("Model modification not allowed!");
    }

    if(node->model.lock() != nullptr){
        throw std::runtime_error("Node belongs to other model!");
    }

    if(isNodeSupported(node) == false){
        throw std::runtime_error("Node type not supported by model!");
    }
    
    if(nodes.find(node) != nodes.end()){
        throw std::runtime_error("Node already added to model!");
    }

    if(node->isInitialized() == false){
        node->initialize();
    }

    node->model = shared_from_this();
    nodes.insert(node);

    afterNodeAdd(node);

    if(node->beginIn() != node->endIn()){
        //dodaj do listy wymaganych pod³¹czenia
        
        PinsSet requiredPins;
        //auto pins = node->getInPins();
        for(auto it = node->beginIn(); it != node->endIn(); it++){
            if((*it)->isRequired() == true){
                requiredPins.insert(*it);
            }
        }

        if(requiredPins.empty() == false){
            //zainicjuj brakuj¹ce piny
            pinsRequiringConnections[node].insert(requiredPins.begin(), requiredPins.end());
        }else{
            //oznacz wêze³ jako niepod³aczony, bo ¿adne wejscie nie ma po³¹czenia
            pinsRequiringConnections[node];
        }

        //oznacz jako liœæ
        addLeaf(node);
    }

    notify();
}

void Model::afterNodeAdd(const NPtr & node)
{

}

void Model::disconnectNode(const NPtr & node)
{
    UTILS_ASSERT((node != nullptr), "B³êdny wêze³");

    ScopedLock lock(editMutex);

    if(isModelChangeAllowed() == false){
        throw std::runtime_error("Model modification not allowed!");
    }

    if(node->model.lock() == nullptr){
        throw std::runtime_error("Node is not assigned to any model!");
    }

    if(node->model.lock().get() != this){
        throw std::runtime_error("Node belongs to other model!");
    }

    quickDisconnectNode(node);

    notify();
}

void Model::quickDisconnectNode(const NPtr & node)
{
    //get all node connections from all pins and remove them
    Connections allNodeConnections;

    for(auto it = node->beginIn(); it != node->endIn(); it++){
        //const Connections & connections = (*it)->getConnections();
        allNodeConnections.insert((*it)->begin(),(*it)->end());
    }

    for(auto it = node->beginOut(); it != node->endOut(); it++){
        //const Connections & connections = (*it)->getConnections();
        allNodeConnections.insert((*it)->begin(),(*it)->end());
    }

    //remove all node connections
    for(auto it = allNodeConnections.begin(); it != allNodeConnections.end(); it++){
        quickRemoveConnection(connections.find(*it));
    }
}

void Model::removeNode(const NPtr & node)
{
    UTILS_ASSERT((node != nullptr), "Bledny wezel");

    ScopedLock lock(editMutex);

    if(isModelChangeAllowed() == false){
        throw std::runtime_error("Model modification not allowed!");
    }

    if(node->model.lock() == nullptr){
        throw std::runtime_error("Node is not assigned to any model!");
    }

    if(node->model.lock().get() != this){
        throw std::runtime_error("Node belongs to other model!");
    }

    quickRemoveNode(nodes.find(node));

    notify();
}

void Model::quickRemoveNode(const Nodes::iterator & nodeIt)
{
    NPtr node(*nodeIt);

    removeLeaf(node);
    beforeNodeRemove(node);

    quickDisconnectNode(node);
    node->model.reset();
    nodes.erase(nodeIt);
    pinsRequiringConnections.erase(node);
}

void Model::beforeNodeRemove(const NPtr & node)
{
    
}

void Model::clearNodes()
{
    ScopedLock lock(editMutex);

    if(isModelChangeAllowed() == false){
        throw std::runtime_error("Model modification not allowed!");
    }

    while(nodes.begin() != nodes.end()) {
        quickRemoveNode(nodes.begin());
    }

    UTILS_ASSERT((connections.size() == 0), "Nie wszystkie po³¹czenia zosta³y usuniête jak siê spodziewano przy usuwaniu wszystkich wêz³ów");

    nodes.swap(Nodes());
    connections.swap(Connections());
    pinsRequiringConnections.swap(RequiringConnection());

    notify();
}

bool Model::additionalConnectRules(const CPinPtr & src, const CPinPtr & dest) const 
{
    return true;
}

bool Model::canConnect(const CPinPtr & src, const CPinPtr & dest) const
{
    ScopedLock lock(editMutex);

	bool ret = true;

	if(src->getType() != Pin::OUT || dest->getType() != Pin::IN || src->getParent() == nullptr || dest->getParent() == nullptr ||
        src->getParent()->model.lock().get() != this || dest->getParent()->model.lock().get() != this ||
        dest->isCompatible(src) == false || findConnection(src, dest) != nullptr){
			ret = false;
	}

    if(ret == true && additionalConnectRules(src, dest) == false){
        ret = false;
    }

	return ret;
}

ConnPtr Model::connect(const PinPtr & src, const PinPtr & dest)
{
    ScopedLock lock(editMutex);

    if(isModelChangeAllowed() == false){
        throw std::runtime_error("Model modification not allowed!");
    }

	if(canConnect(src, dest) != true){
		throw std::runtime_error("Connection can not be created - breaks basic connectivity rules!");
	}

    ConnPtr ret(new Connection(src, dest));
    connections.insert(ret);
    src->addConnection(ret);
    dest->addConnection(ret);

    //jeœli Ÿród³o jest zale¿ne od jakichœ pinów wejœciowych w obrêbie ich wêz³a i s¹ one niepod³¹czone, to trzeba je dodaæ jako wymagane
    PinsSet requiredPins;
    for(auto it = src->getDependantPins().begin(); it != src->getDependantPins().end(); it++){
        if((*it).lock()->empty() == true){
            requiredPins.insert(*it);
        }
    }

    if(requiredPins.empty() == false){
        pinsRequiringConnections[src->getParent()].insert(requiredPins.begin(), requiredPins.end());
    }


    //jeœli cel jest na liœcie wymagaj¹cych podpiêcia usuñ go
    auto it = pinsRequiringConnections.find(dest->getParent());
    if(it != pinsRequiringConnections.end()){
        it->second.erase(dest);

        //sprawdŸ czy wêze³ poprawnie pod³¹czony
        if(it->second.empty() == true){
            pinsRequiringConnections.erase(it);
        }
    }

    afterConnect(ret);

    if(leafNodes.find(src->getParent()) != leafNodes.end()){
        removeLeaf(src->getParent());
    }

    notify();

    return ret;
}

void Model::afterConnect(const ConnPtr & connection)
{

}

void Model::removeConnection(const ConnPtr & connection)
{
    ScopedLock lock(editMutex);

    if(isModelChangeAllowed() == false){
        throw std::runtime_error("Model modification not allowed!");
    }

    auto it = connections.find(connection);
	if(it == connections.end()){
		throw std::runtime_error("Connection dos not exist in model!");
	}

    quickRemoveConnection(it);

    notify();
}

void Model::clearConnections()
{
    ScopedLock lock(editMutex);

    if(isModelChangeAllowed() == false){
        throw std::runtime_error("Model modification not allowed!");
    }
    
    while(connections.begin() != connections.end()){
        quickRemoveConnection(connections.begin());
    }

    connections.swap(Connections());

    notify();
}

void Model::quickRemoveConnection(const Connections::iterator & connIt)
{
    ConnPtr connection(*connIt);

    beforeRemoveConnection(connection);

    //policz ile po³¹czeñ dochodzi do wêz³a docelowego usuwanego po³¹czenia (wystarczy ¿e bêdzie ich wiêcej ni¿ 1)
    //sprawdŸ czy docelowy pin nie jest wymagany dla jakiegoœ po³¹czonego piny wyjœciowego wêz³a
    PinPtr destPin(connection->getDest());
    NPtr destNode(destPin->getParent());
    NPtr srcNode(connection->getSrc()->getParent());
    unsigned int countIn = 0;
    bool otherDepends = false;
    for(auto it = destNode->beginIn(); it != destNode->endIn(); it++){
        if((*it)->empty() == false){
            if( otherDepends == false &&
                (*it)->getDependantPins().find(destPin) != (*it)->getDependantPins().end()){
                    otherDepends = true;
            }
            countIn += (*it)->size();
            if(countIn > 1){
                break;
            }
        }
    }

    if(countIn == 1){
        //to ostatnie po³¹czenie, wiêc zostanie niepod³¹czony
        pinsRequiringConnections[destNode];
    }

    if(otherDepends == true || destPin->isRequired() == true){
        //ten pin by³ albo wymagany albo zale¿¹ od niego inne piny wyjœciowe wêz³a
        pinsRequiringConnections[destNode].insert(destPin);
    }

    connection->getSrc()->removeConnection(connection);
    connection->getDest()->removeConnection(connection);
    connection->setDest(PinPtr());
    connection->setSrc(PinPtr());
    connections.erase(connIt);

    //sprawdŸ czy nie powsta³ nowy liœæ
    if(srcNode->beginIn() != srcNode->endIn()){
        bool leaf = true;
        for(auto it = srcNode->beginOut(); it != srcNode->endOut(); it++){
            if((*it)->empty() == false){
                leaf = false;
                break;
            }
        }

        if(leaf == true){
            addLeaf(srcNode);
        }
    }
}

void Model::beforeRemoveConnection(const ConnPtr & connection)
{

}

bool Model::createCycle(const CPinPtr & src, const CPinPtr & dest) const
{
    return !getCycle(src, dest).empty();
}

Model::CyclePath Model::getCycle(const CPinPtr & src, const CPinPtr & dest) const
{
    ScopedLock lock(editMutex);
	//if source is connected no loops possible
	//also if leaf node is connected

	//connections in path
	CyclePath connInPath;

	NPtr destNode = dest->getParent();
	if(Node::anyOutPinConnected(destNode) == false){
		return connInPath;
	}

	PathEntry pe = getFirstNodeOutputConnection(destNode);

	//stores node compleately checked - up to leafs
	Nodes checkedNodes;

    initCycleCheckedNodes(checkedNodes);

	//nodes in path
	Nodes nodesInPath;

	nodesInPath.insert(src->getParent());
	nodesInPath.insert(destNode);

	//structure for traversing
	std::vector<PathEntry> path;
	
	//find first output connection
	path.push_back(pe);
	//add it co connections
	connInPath.push_back(pe.node->getOutPin(pe.pinIndex)->getConnection(pe.connectionIndex));

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
				connInPath.push_back(pe.node->getOutPin(pe.pinIndex)->getConnection(pe.connectionIndex));
			}

			continue;
		}

		//go to the next node!!
		//update nodes in path
		nodesInPath.insert(nextNode);

		path.push_back(getFirstNodeOutputConnection(nextNode));
		connInPath.push_back(path.back().node->getOutPin(path.back().pinIndex)->getConnection(path.back().connectionIndex));
	}

	return connInPath;
}

Model::PathEntry Model::getFirstNodeOutputConnection(const NPtr & node)
{
    PathEntry pathElement;

    for(unsigned int i = 0; i <= node->sizeOut(); i++){
        for(unsigned int j = 0; j < node->getOutPin(i)->size(); j++){
            pathElement.node = node;
            pathElement.pinIndex = i;
            pathElement.connectionIndex = j;
            //pathElement.pinIT = it;
            //pathElement.connIT = iT;
            return pathElement;
        }
    }

    return pathElement;
}

Model::PathEntry Model::getNextNodeOutputConnection(const PathEntry & pathElement)
{
    PathEntry pe = pathElement;

    if(++pe.connectionIndex < static_cast<int>(pathElement.node->getOutPin(pathElement.pinIndex)->size())){
        return pe;
    }

    pe.pinIndex++;

    for( ; pe.pinIndex < static_cast<int>(pe.node->sizeOut()); pe.pinIndex++){
        if(pe.node->getOutPin(pe.pinIndex)->empty() == false){
            pe.connectionIndex = 0;
            return pe;
        }
    }

    return pathElement;
}

bool Model::PathEntry::operator==(const PathEntry & pe) const
{
    if(this->node == pe.node && this->pinIndex == pe.pinIndex && this->connectionIndex == pe.connectionIndex){
        return true;	
    }

    return false;
}

void Model::initCycleCheckedNodes(Model::Nodes & nodes) const
{
    nodes.insert(leafNodes.begin(), leafNodes.end());
}

ConnPtr Model::findConnection(CPinPtr src, CPinPtr dest) const
{
	ConnPtr ret;

	if(src->size() > dest->size()){
		src.swap(dest);
	}

	for(auto it = src->begin(); it != src->end(); it++){
		//connection exist and is registered by the model
		if((*it)->getOther(src) == dest && std::find(connections.begin(), connections.end(), *it) != connections.end()){
			ret = *it;
			break;
		}
	}

	return ret;
}

void Model::addLeaf(const NPtr & node)
{
    //check if node is not a leafe already
    if(leafNodes.find(node) == leafNodes.end()){
        leafNodes.insert(node);
        afterLeafAdd(node);
    }
}

void Model::removeLeaf(const NPtr & node)
{
    auto it = leafNodes.find(node);
    if(it != leafNodes.end()){
        beforeLeafRemove(node);
        //DFNode::getDFNode(node)->onLeafProcessedCallback.clear();
        leafNodes.erase(node);
    }
}

void Model::afterLeafAdd(const NPtr & node)
{

}

void Model::beforeLeafRemove(const NPtr & node)
{

}

}