#include <dfmlib/Connection.h>
#include <dfmlib/Node.h>
#include <dfmlib/Pin.h>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

Connection::Connection(PinPtr src, PinPtr dest) : srcPin(src), destPin(dest){

}

Connection::~Connection(void)
{
}

PinPtr Connection::getSrc() const{
	if(srcPin.expired() == false){
		return srcPin.lock();
	}

	return PinPtr();
}

PinPtr Connection::getDest() const{
	if(destPin.expired() == false){
		return destPin.lock();
	}

	return PinPtr();
}

PinPtr Connection::getOther(CWPinPtr pin) const{

	if(pin.expired() == true){
		return PinPtr();
	}

	PinPtr ret = getSrc();

	if(pin.lock() == ret){
		ret = getDest();
	}

	return ret;
}

std::string Connection::getConnectionName() const{
	std::string ret("Empty Connection");

	if(srcPin.expired() != true && destPin.expired() != true &&
		srcPin.lock()->getParent() != 0 && destPin.lock()->getParent() != 0){
			ret = srcPin.lock()->getParent()->getNodeName();
			ret.append(":");
			ret.append(srcPin.lock()->getPinName());
			ret.append(" - ");
			ret.append(destPin.lock()->getParent()->getNodeName());
			ret.append(":");
			ret.append(destPin.lock()->getPinName());
	}

	return ret;
}

void Connection::setSrc(PinPtr src){
	srcPin = src;
}

void Connection::setDest(PinPtr dest){
	destPin = dest;
}

}