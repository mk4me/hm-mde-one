#include <dfmlib/Pin.h>
#include <dfmlib/Connection.h>
#include <dfmlib/Node.h>
#include <dfmlib/Model.h>
#include <typeinfo>
#include <boost/assert.hpp>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

Pin::Pin(WNPtr parentNode, const std::string & pinName, bool required,
	const Pin::REQ_PINS_SET & requiredPins, const Pin::CONNECTIONS_SET & connections)
	: boost::enable_shared_from_this<Pin>(), parentNode(parentNode),
	pinType(PIN_UNKNOWN), pinRequired(required),	requiredPinsDependency(requiredPins),
	connections(connections), pinName(pinName){

	BOOST_ASSERT((parentNode.expired() == false && parentNode.lock() != 0));
}

Pin::~Pin(void){

}

NPtr Pin::getParent() const{
	return parentNode.lock();
}
const Pin::CONNECTIONS_SET & Pin::getConnections() const{
	return connections;
}

const Pin::REQ_PINS_SET & Pin::getDependantPins() const{
	return requiredPinsDependency;
}

Pin::PIN_TYPE Pin::getType() const {
	return pinType;
}

void Pin::setType(PIN_TYPE type){
	pinType = type;
}

bool Pin::isCompatible(CPinPtr pin) const{
	return typeid(*this) == typeid(*pin);
}

bool Pin::addConnection(ConnPtr cn){
	connections.insert(cn);
	return true;
}

bool Pin::removeConnection(ConnPtr cn){
	connections.erase(cn);
	return true;
}

void Pin::clearConnections(){
	connections.clear();
}

const std::string & Pin::getPinName() const{
	return pinName;
}

void Pin::setPinName(const std::string & pinName){
	this->pinName = pinName;
}

bool Pin::dependsOnPin(PinPtr pin) const{
	if(requiredPinsDependency.find(pin) != requiredPinsDependency.end()){
		return true;
	}

	return false;
}

bool Pin::isRequired() const{
	return pinRequired;
}

bool Pin::isComplete() const{
	bool ret = true;
	for(REQ_PINS_SET::const_iterator it = requiredPinsDependency.begin(); it != requiredPinsDependency.end(); it++){
		if((*it).lock()->getConnections().empty() == true){
			ret = false;
			break;
		}
	}

	return ret;
}

Pin::PIN_STATIC_STATUS Pin::getStaticStatus() const{
	PIN_STATIC_STATUS ret = PIN_OK;

	switch(pinType){

		case PIN_IN:
			if(connections.empty() == false){
				ret = PIN_CONNECTED;
			}else if(pinRequired == true){
				ret = PIN_REQUIRED;
			}
			break;

		case PIN_OUT:
			if(isComplete() == false){
				ret = PIN_INCOMPLETE;
			}else if(connections.empty() == false){
				ret = PIN_CONNECTED;
			}
			break;
	}

	return ret;
}

Pin::PIN_DYNAMIC_STATUS Pin::getDynamicStatus(PinPtr refPin, MPtr model) const{
	PIN_DYNAMIC_STATUS ret = PIN_COMPATIBLE;

	if(pinType == refPin->pinType || (pinType == PIN_IN && isCompatible(refPin) == false) ||
		(pinType == PIN_OUT && refPin->isCompatible(shared_from_this()) == false)){
		ret = PIN_INCOMPATIBLE;
	}else if(pinType == PIN_IN && model->createCycle(refPin, shared_from_this()) == true){
		ret = PIN_COMPATIBLE_CYCLE;
	}else if(pinType == PIN_OUT && model->createCycle(shared_from_this(), refPin) == true){
		ret = PIN_COMPATIBLE_CYCLE;
	}

	return ret;
}

}