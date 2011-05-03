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

DFNPtr DFNode::getDFNode(const NPtr & node)
{
	return boost::dynamic_pointer_cast<DFNode>(node);
}

void DFNode::configureNode()
{

}

void DFNode::allowProcessing(bool allow)
{
    if(processingAllowed == allow){
        return;
    }

    processingAllowed = allow;
}

void DFNode::allowPropagating(bool allow)
{
    if(propagatingAllowed == allow){
        return;
    }

    propagatingAllowed = allow;
    if(allow == true){
        notify();
    }
}

bool DFNode::isProcessingAllowed() const
{
	return processingAllowed;
}

bool DFNode::isPropagatingAllowed() const
{
	return propagatingAllowed;
}


bool DFNode::checkInputPins() const
{
	for(PINS_SET::const_iterator it = inPins.begin(); it != inPins.end(); it++){
		const Pin::CONNECTIONS_SET & connections = (*it)->getConnections();
		for(Pin::CONNECTIONS_SET::const_iterator iT = connections.begin(); iT != connections.end(); iT++){
			DFPinPtr ptr(DFPin::getDFPin((*iT)->getOther(*it)));
			if( ptr != nullptr && ptr->isUpdated() == false){
				return false;
			}
		}
	}

	return true;
}

void DFNode::updateOutputPins()
{
	for(PINS_SET::iterator it = outPins.begin(); it != outPins.end(); it++){
		DFPinPtr pin(DFPin::getDFPin(*it));
		
		if(pin != nullptr){
			pin->update();		
			const Pin::CONNECTIONS_SET & connections = (*it)->getConnections();
			//inform input pins on the following nodes
			for(Pin::CONNECTIONS_SET::const_iterator iT = connections.begin(); iT != connections.end(); iT++){
				pin = boost::dynamic_pointer_cast<DFPin>((*iT)->getOther(*it));
				if(pin != nullptr){
					pin->update();
				}
			}
		}
	}
}

void DFNode::setOutputData()
{

}

void DFNode::processData()
{

}

void DFNode::process()
{
	if(checkInputPins() == true){

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