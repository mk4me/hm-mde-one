#include <plugins/dfElements/DFPins.h>


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


//void UniversalInputPinBase::copyData( const df::IDFOutput * pin )
//{
//  auto scalarPin = dynamic_cast<const ScalarOutputPin*>(pin);
//  if (scalarPin) {
//      scalar = scalarPin->value();
//  } else {
//      auto vectorPin = dynamic_cast<const VectorOutputPin*>(pin);
//      vector = vectorPin ? vectorPin->value() : VectorChannelReaderInterfaceConstPtr();
//  }
//}

//void VectorOutputPin::reset()
//{
//    val = VectorChannelReaderInterfaceConstPtr();
//}
//
//void VectorOutputPin::value( VectorChannelReaderInterfaceConstPtr val )
//{
//    this->val = val;
//}
//
//const VectorChannelReaderInterfaceConstPtr VectorOutputPin::value() const
//{
//    return val;
//}
//
//VectorOutputPin::VectorOutputPin( df::ISourceNode * node ) : df::OutputPin(node)
//{
//
//}
//
//const VectorChannelReaderInterfaceConstPtr VectorInputPin::value() const
//{
//    return val;
//}
//
//void VectorInputPin::reset()
//{
//    val = VectorChannelReaderInterfaceConstPtr();
//}
//
//void VectorInputPin::copyData( const df::IDFOutput * pin )
//{
//    val = dynamic_cast<const VectorOutputPin*>(pin)->value();
//}
//
//VectorInputPin::VectorInputPin( df::ISinkNode * node ) : df::InputPin(node)
//{
//
//}
//
//const bool VectorInputPin::pinCompatible( const df::IOutputPin * pin ) const
//{
//    return dynamic_cast<const VectorOutputPin*>(pin);
//}
//
////const ScalarChannelReaderInterfaceConstPtr ChannelInputPin::value() const
////{
////    return val;
////}
//
//void ChannelInputPin::reset()
//{
//    scalar = ScalarChannelReaderInterfaceConstPtr(); 
//    vector = VectorChannelReaderInterfaceConstPtr();
//}
//
//void ChannelInputPin::copyData( const df::IDFOutput * pin )
//{
//  auto scalarPin = dynamic_cast<const ScalarOutputPin*>(pin);
//  if (scalarPin) {
//      scalar = scalarPin->value();
//  } else {
//      auto vectorPin = dynamic_cast<const VectorOutputPin*>(pin);
//      vector = vectorPin ? vectorPin->value() : VectorChannelReaderInterfaceConstPtr();
//  }
//}
//
//ChannelInputPin::ChannelInputPin( df::ISinkNode * node ) : df::InputPin(node)
//{
//
//}
//
//const bool ChannelInputPin::pinCompatible( const df::IOutputPin * pin ) const
//{
//    return dynamic_cast<const ScalarOutputPin*>(pin) || dynamic_cast<const VectorOutputPin*>(pin);
//}
//
//const bool ScalarInputPin::pinCompatible( const df::IOutputPin * pin ) const
//{
//    return dynamic_cast<const ScalarOutputPin*>(pin);
//}


void UniversalOutputPinBase::reset()
{
    val = ConstPtr();
}

void UniversalOutputPinBase::setWrapper( ConstPtr val )
{
    this->val = val;
}

const UniversalOutputPinBase::ConstPtr UniversalOutputPinBase::getWrapper() const
{
    return val;
}

UniversalOutputPinBase::UniversalOutputPinBase( df::ISourceNode * node ) : 
df::OutputPin(node)
{

}

utils::TypeInfo UniversalOutputPin::getInfo() const
{
    return info;
}

UniversalOutputPin::~UniversalOutputPin()
{

}

UniversalOutputPin::UniversalOutputPin( df::ISourceNode* node, const utils::TypeInfo& info ) : 
UniversalOutputPinBase(node),
    info(info)
{

}
