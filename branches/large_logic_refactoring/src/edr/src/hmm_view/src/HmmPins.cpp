#include "hmmPCH.h"
#include "HmmPins.h"
#include "HmmMainWindow.h"

void XOutputPin::reset()
{
    val = VectorChannelReaderInterfaceConstPtr();
}

void XOutputPin::value( VectorChannelReaderInterfaceConstPtr val )
{
    this->val = val;
}

const VectorChannelReaderInterfaceConstPtr XOutputPin::value() const
{
    return val;
}

XOutputPin::XOutputPin( df::ISourceNode * node ) : df::OutputPin(node)
{

}

const VectorChannelReaderInterfaceConstPtr XInputPin::value() const
{
    return val;
}

void XInputPin::reset()
{
    val = VectorChannelReaderInterfaceConstPtr();
}

void XInputPin::copyData( const df::IDFOutput * pin )
{
    val = dynamic_cast<const XOutputPin*>(pin)->value();
}

XInputPin::XInputPin( df::ISinkNode * node ) : df::InputPin(node)
{

}
