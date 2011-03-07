#include <dfmlib/DFNode.h>
#include <dfmlib/Pin.h>
#include <dfmlib/DFPin.h>
#include <dfmlib/Connection.h>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

DFNode::DFNode(const std::string & nodeName, bool processingAllowed, bool propagatingAllowed)
	: Node(nodeName), processingAllowed(processingAllowed),
	propagatingAllowed(propagatingAllowed)
{
}


DFNode::~DFNode(void)
{
}

DFNPtr DFNode::getDFNode(NPtr node){
	return boost::dynamic_pointer_cast<DFNode>(node);
}

bool DFNode::setEnable(bool enable){
	objectEnable = enable;
	notify();
	return true;
}

void DFNode::configureNode() {

}

bool DFNode::isProcessingAllowed() const{
	return processingAllowed;
}

bool DFNode::isPropagatingAllowed() const{
	return propagatingAllowed;
}

bool DFNode::addInPin(PinPtr newPin){
	if(DFPin::isDFPin(newPin) == false){
		return false;
	}

	return Node::addInPin(newPin);
}

bool DFNode::addOutPin(PinPtr newPin){
	if(DFPin::isDFPin(newPin) == false){
		return false;
	}

	return Node::addOutPin(newPin);
}

bool DFNode::checkInputPins() const{

	for(PINS_SET::const_iterator it = inPins.begin(); it != inPins.end(); it++){
		const Pin::CONNECTIONS_SET & connections = (*it)->getConnections();
		for(Pin::CONNECTIONS_SET::const_iterator iT = connections.begin(); iT != connections.end(); iT++){
			DFPinPtr ptr(boost::dynamic_pointer_cast<DFPin>((*iT)->getOther(*it)));
			if( ptr != 0 && ptr->isUpdated() == false){
				return false;
			}
		}
	}

	return true;
}

void DFNode::updateOutputPins(){
	for(PINS_SET::iterator it = outPins.begin(); it != outPins.end(); it++){
		DFPinPtr pin(boost::dynamic_pointer_cast<DFPin>(*it));
		
		if(pin != 0){
			pin->update();		
			const Pin::CONNECTIONS_SET & connections = (*it)->getConnections();
			//inform input pins on the following nodes
			for(Pin::CONNECTIONS_SET::const_iterator iT = connections.begin(); iT != connections.end(); iT++){
				pin = boost::dynamic_pointer_cast<DFPin>((*iT)->getOther(*it));
				if(pin != 0){
					pin->update();
				}
			}
		}
	}
}

void DFNode::getInputData(){
}

void DFNode::setOutputData(){
}

void DFNode::processData(){
}

void DFNode::process(){
	if(checkInputPins() == true){
		getInputData();

		if(processingAllowed == true){
			processData();
		}

		if(propagatingAllowed == true){	
			setOutputData();
			updateOutputPins();
		}
	}
}

}