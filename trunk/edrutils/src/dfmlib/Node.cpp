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

bool Node::addInPin(PinPtr newPin){
    UTILS_ASSERT((newPin != nullptr), "Nieprawidlowy pin");
    UTILS_ASSERT((newPin->parentNode.lock() == nullptr), "Pin ma juz rodzica");
	
    newPin->parentNode = shared_from_this();
    inPins.insert(newPin);
	newPin->setType(Pin::PIN_IN);
	return true;
}

bool Node::addOutPin(PinPtr newPin){
    UTILS_ASSERT((newPin != nullptr), "Nieprawidlowy pin");
    UTILS_ASSERT((newPin->parentNode.lock() == nullptr), "Pin ma juz rodzica");
	
    newPin->parentNode = shared_from_this();
    outPins.insert(newPin);
	newPin->setType(Pin::PIN_OUT);
	return true;
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

bool Node::pinExists(PinPtr pin) const {
	return pinExists(inPins,pin) || pinExists(outPins,pin);
}

bool Node::validInPinsConnection() const{
	bool ret = true;
	bool empty = true;
	bool required = false;

	for(PINS_SET::const_iterator it = inPins.begin(); it != inPins.end(); it++){
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

bool Node::validOutPinsConnection() const{
	bool ret = true;

	for(PINS_SET::const_iterator it = outPins.begin(); it != outPins.end(); it++){
		if((*it)->isComplete() == false){
			ret = false;
			break;
		}
	}

	return ret;
}

bool Node::validConnection() const{
	return validInPinsConnection() && validOutPinsConnection();
}

bool Node::anyInPinConnected() const{
	return anyPinConnected(inPins);
}

bool Node::anyOutPinConnected() const{
	return anyPinConnected(outPins);
}

bool Node::anyPinConnected(const PINS_SET & pins){
	bool ret = false;

	for(PINS_SET::const_iterator it = pins.begin(); it != pins.end(); it++){
		if((*it)->getConnections().empty() == false){
			ret = true;
			break;
		}
	}

	return ret;
}

bool Node::anyPinConnected() const{
	return anyInPinConnected() && anyOutPinConnected();
}
}