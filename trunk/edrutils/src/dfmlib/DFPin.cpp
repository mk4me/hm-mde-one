#include <dfmlib/DFPin.h>
#include <dfmlib/DFNode.h>
#include <dfmlib/Connection.h>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

DFPin::DFPin(const std::string & nodeName, bool required,
	const Pin::REQ_PINS_SET & requiredPins)
	: Pin(nodeName, required, requiredPins),
		updated(false)
{

}

DFPin::~DFPin(void)
{

}

bool DFPin::isDFPin(CPinPtr pin)
{
	if(getDFPin(pin) != nullptr){
		return true;
	}

	return false;
}

DFPinPtr DFPin::getDFPin(PinPtr pin)
{
	return boost::dynamic_pointer_cast<DFPin>(pin);
}

CDFPinPtr DFPin::getDFPin(CPinPtr pin)
{
    return boost::dynamic_pointer_cast<const DFPin>(pin);
}

bool DFPin::isUpdated() const
{
	return updated;
}

void DFPin::reset()
{
	updated = false;
}

void DFPin::update()
{
	updated = true;
    PIN_TYPE pinType = getType();
	if(pinType == PIN_IN){
		DFNPtr node(DFNode::getDFNode(getParent()));
		if(node != nullptr){
			node->notify();
		}
	}else if(pinType == PIN_OUT){
        const CONNECTIONS_SET & connections = getConnections();
		for(CONNECTIONS_SET::const_iterator it = connections.begin(); it != connections.end(); it++){
			DFPinPtr pin(boost::dynamic_pointer_cast<DFPin>((*it)->getDest()));
			if(pin != nullptr){
				pin->copyDataFromPin(boost::dynamic_pointer_cast<DFPin>(shared_from_this()));
			}
		}
	}

	onUpdate();
}

void DFPin::copyDataFromPin(DFPinPtr pin)
{

}

void DFPin::onUpdate()
{

}

}