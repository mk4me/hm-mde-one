#include "NewVdfPCH.h"
#include <plugins/newVdf/UniversalInputPin.h>
#include <plugins/newVdf/UniversalOutputPin.h>

using namespace vdf;

UniversalInputPinBase::UniversalInputPinBase( df::ISinkNode * node ) : 
    df::InputPin(node)
{

}

const bool UniversalInputPin::pinCompatible( const df::IOutputPin * pin ) const
{
    const UniversalOutputPin* op = dynamic_cast<const UniversalOutputPin*>(pin);
    if (op) {
        return op->getInfo() == this->info;
    }

    return false;
}

UniversalInputPin::UniversalInputPin( df::ISinkNode* node, const utils::TypeInfo& info ) : 
UniversalInputPinBase(node),
    info(info)
{

}

utils::TypeInfo UniversalInputPin::getInfo() const
{
    return info;
}

UniversalInputPin::~UniversalInputPin()
{

}

void UniversalInputPinBase::setWrapper( utils::ObjectWrapperConstPtr val )
{
    wrapper = val;
}

utils::ObjectWrapperConstPtr UniversalInputPinBase::getWrapper() const
{
    return wrapper;
}

const bool UniversalInputPinBase::pinCompatible( const df::IOutputPin * pin ) const
{
    return dynamic_cast<const UniversalOutputPinBase*>(pin);
}

void UniversalInputPinBase::copyData( const df::IDFOutput * pin )
{
    auto uniPin = dynamic_cast<const UniversalOutputPinBase*>(pin);
    UTILS_ASSERT(uniPin);
    wrapper = uniPin->getWrapper()->clone();
}

void UniversalInputPinBase::reset()
{
    wrapper = utils::ObjectWrapperPtr();
}


