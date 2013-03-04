#include "hmmPCH.h"
#include "HmmPins.h"
#include "HmmMainWindow.h"

void VectorOutputPin::reset()
{
    val = VectorChannelReaderInterfaceConstPtr();
}

void VectorOutputPin::value( VectorChannelReaderInterfaceConstPtr val )
{
    this->val = val;
}

const VectorChannelReaderInterfaceConstPtr VectorOutputPin::value() const
{
    return val;
}

VectorOutputPin::VectorOutputPin( df::ISourceNode * node ) : df::OutputPin(node)
{

}

const VectorChannelReaderInterfaceConstPtr VectorInputPin::value() const
{
    return val;
}

void VectorInputPin::reset()
{
    val = VectorChannelReaderInterfaceConstPtr();
}

void VectorInputPin::copyData( const df::IDFOutput * pin )
{
    val = dynamic_cast<const VectorOutputPin*>(pin)->value();
}

VectorInputPin::VectorInputPin( df::ISinkNode * node ) : df::InputPin(node)
{

}

const bool VectorInputPin::pinCompatible( const df::IOutputPin * pin ) const
{
    return dynamic_cast<const VectorOutputPin*>(pin);
}

//const ScalarChannelReaderInterfaceConstPtr ChannelInputPin::value() const
//{
//    return val;
//}

void ChannelInputPin::reset()
{
    scalar = ScalarChannelReaderInterfaceConstPtr(); 
    vector = VectorChannelReaderInterfaceConstPtr();
}

void ChannelInputPin::copyData( const df::IDFOutput * pin )
{
  auto scalarPin = dynamic_cast<const ScalarOutputPin*>(pin);
  if (scalarPin) {
      scalar = scalarPin->value();
  } else {
      auto vectorPin = dynamic_cast<const VectorOutputPin*>(pin);
      vector = vectorPin ? vectorPin->value() : VectorChannelReaderInterfaceConstPtr();
  }
}

ChannelInputPin::ChannelInputPin( df::ISinkNode * node ) : df::InputPin(node)
{

}

const bool ChannelInputPin::pinCompatible( const df::IOutputPin * pin ) const
{
    return dynamic_cast<const ScalarOutputPin*>(pin) || dynamic_cast<const VectorOutputPin*>(pin);
}

const bool ScalarInputPin::pinCompatible( const df::IOutputPin * pin ) const
{
    return dynamic_cast<const ScalarOutputPin*>(pin);
}
