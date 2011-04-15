#include <dfmlib/DFSourceNode.h>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

DFSourceNode::DFSourceNode(const std::string & nodeName, bool processingAllowed,
	bool propagatingAllowed) : DFNode(nodeName,processingAllowed,propagatingAllowed)
{
}


DFSourceNode::~DFSourceNode(void)
{
}

bool DFSourceNode::addInPin(PinPtr pin){
	return false;
}

bool DFSourceNode::checkInputPins() const{
	return true;
}

void DFSourceNode::resetInputPins(){
}

}