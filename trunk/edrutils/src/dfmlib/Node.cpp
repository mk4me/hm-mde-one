#include <dfmlib/Node.h>
#include <dfmlib/Pin.h>
#include <utils/Debug.h>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

Node::Node(const std::string & nodeName) : nodeName(nodeName)
{
}


Node::~Node(void)
{
}

const std::string & Node::getNodeName() const{
	return nodeName;
}

void Node::setNodeName(const std::string & nodeName){
	this->nodeName = nodeName;
}

void Node::addInPin( PinPtr newPin )
{
    UTILS_ASSERT((newPin != nullptr), "Nieprawidlowy pin");
    if(newPin->parentNode.lock() != nullptr){
        throw std::runtime_error("Pin is already connected with node!");
    }
	
    newPin->parentNode = shared_from_this();
    inPins.insert(newPin);
	newPin->setType(Pin::PIN_IN);
}

void Node::addOutPin( PinPtr newPin )
{
    UTILS_ASSERT((newPin != nullptr), "Nieprawidlowy pin");
    if(newPin->parentNode.lock() != nullptr){
        throw std::runtime_error("Pin is already connected with node!");
    }
	
    newPin->parentNode = shared_from_this();
    outPins.insert(newPin);
	newPin->setType(Pin::PIN_OUT);
}

const Node::PINS_SET & Node::getInPins() const{
	return inPins;
}

const Node::PINS_SET & Node::getOutPins() const{
	return outPins;
}

bool Node::pinExists(const std::set<PinPtr> & pins, PinPtr pin){
	bool ret = false;

	if(pins.find(pin) != pins.end()){
		ret = true;
	}

	return ret;
}

bool Node::pinExists(const CNPtr & node, PinPtr pin) {
	return pinExists(node->getInPins(),pin) || pinExists(node->getOutPins(),pin);
}

bool Node::validInPinsConnection(const CNPtr & node) {
	bool ret = true;
	bool empty = true;
	bool required = false;

    const PINS_SET & inPins = node->getInPins();

	for(auto it = inPins.begin(); it != inPins.end(); it++){
		if((*it)->isRequired() == true){
			required = true;
			if((*it)->getConnections().empty()){
				ret = false;
				break;
			}
		}else{
			empty &= (*it)->getConnections().empty();
		}
	}

	if(required == false && empty == true){
		ret = false;
	}

	return ret;
}

bool Node::validOutPinsConnection(const CNPtr & node) {
	bool ret = true;

    const PINS_SET & outPins = node->getOutPins();

	for(auto it = outPins.begin(); it != outPins.end(); it++){
		if((*it)->isComplete() == false){
			ret = false;
			break;
		}
	}

	return ret;
}

bool Node::validConnection(const CNPtr & node){
	return validInPinsConnection(node) && validOutPinsConnection(node);
}

bool Node::anyInPinConnected(const CNPtr & node){
	return anyPinConnected(node->getInPins());
}

bool Node::anyOutPinConnected(const CNPtr & node){
	return anyPinConnected(node->getOutPins());
}

bool Node::anyPinConnected(const PINS_SET & pins){
	bool ret = false;

	for(auto it = pins.begin(); it != pins.end(); it++){
		if((*it)->getConnections().empty() == false){
			ret = true;
			break;
		}
	}

	return ret;
}

bool Node::anyPinConnected(const CNPtr & node){
	return anyInPinConnected(node) && anyOutPinConnected(node);
}
}