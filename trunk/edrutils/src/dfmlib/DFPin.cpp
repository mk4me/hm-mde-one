#include <dfmlib/DFPin.h>
#include <dfmlib/DFNode.h>
#include <dfmlib/Connection.h>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

DFPin::DFPin(WDFNPtr parentNode, const std::string & nodeName, bool required,
	const Pin::REQ_PINS_SET & requiredPins,	const Pin::CONNECTIONS_SET & connections)
	: Pin(parentNode, nodeName, required, requiredPins, connections),
		updated(false) {
}

DFPin::~DFPin(void)
{
}

bool DFPin::isDFPin(PinPtr pin){
	if(getDFPin(pin) != 0){
		return true;
	}

	return false;
}

DFPinPtr DFPin::getDFPin(PinPtr pin){
	return boost::dynamic_pointer_cast<DFPin>(pin);
}

bool DFPin::isUpdated() const{
	return updated;
}

void DFPin::reset(){
	updated = false;
}

bool DFPin::update(){
	updated = true;

	if(pinType == PIN_IN){
		DFNPtr node(boost::dynamic_pointer_cast<DFNode>(getParent()));
		if(node != 0){
			node->notify();
		}
	}else if(pinType == PIN_OUT){
		for(CONNECTIONS_SET::const_iterator it = connections.begin(); it != connections.end(); it++){
			DFPinPtr pin(boost::dynamic_pointer_cast<DFPin>((*it)->getDest()));
			if(pin != 0){
				pin->copyDataFromPin(boost::dynamic_pointer_cast<DFPin>(shared_from_this()));
			}
		}
	}

	return onUpdate();
}

void DFPin::copyDataFromPin(DFPinPtr pin){
}

bool DFPin::onUpdate(){
	return true;
}

}