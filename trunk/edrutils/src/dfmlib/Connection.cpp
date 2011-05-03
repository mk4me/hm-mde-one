#include <dfmlib/Connection.h>
#include <dfmlib/Node.h>
#include <dfmlib/Pin.h>
#include <utils/Debug.h>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

Connection::Connection(PinPtr src, PinPtr dest) : srcPin(src), destPin(dest)
{
    UTILS_ASSERT((src != nullptr), "Bledny pin zrodlowy");
    UTILS_ASSERT((dest != nullptr), "Bledny pin docelowy");
}

Connection::~Connection(void)
{

}

PinPtr Connection::getSrc() const
{
	if(srcPin.expired() == true){
		throw std::runtime_error("Connection based on expired source pin!");
	}

	return srcPin.lock();
}

PinPtr Connection::getDest() const
{
	if(destPin.expired() == true){
		throw std::runtime_error("Connection based on expired destination pin!");
	}

	return destPin.lock();
}

PinPtr Connection::getOther(CWPinPtr pin) const
{
	PinPtr ret = getSrc();

	if(pin.lock() == ret){
		ret = getDest();
	}

	return ret;
}

std::string Connection::getConnectionName() const{
	std::string ret("Empty Connection");

	if(srcPin.lock() != 0 && destPin.lock() != 0){
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
    UTILS_ASSERT((src != nullptr), "Bledny pin zrodlowy");
	srcPin = src;
}

void Connection::setDest(PinPtr dest){
    UTILS_ASSERT((dest != nullptr), "Bledny pin docelowy");
	destPin = dest;
}

}