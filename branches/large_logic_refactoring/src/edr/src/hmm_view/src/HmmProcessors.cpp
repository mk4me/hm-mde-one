#include "hmmPCH.h"
#include "HmmProcessors.h"
#include "HmmMainWindow.h"

void XProcessor::process()
{
    VectorChannelReaderInterfaceConstPtr signal1 = inPinA->value();
    VectorChannelReaderInterfaceConstPtr signal2 = inPinB->value();

    UTILS_ASSERT(signal1 && signal2);
    VectorChannelPtr channel(new VectorChannel(signal1->size() / signal1->getLength()));
    size_type count = (std::min)(signal1->size(), signal2->size());

    for (size_type i = 0; i < count; ++i) {
        auto val = signal1->value(i) - signal2->value(i);
        channel->addPoint(val);
    }
    outPinA->value(channel);
}

void XProcessor::reset()
{

}

XProcessor::XProcessor()
{
    inPinA = new XInputPin(this);
    inPinB = new XInputPin(this);
    outPinA = new XOutputPin(this);
    addInputPin(inPinA);
    addInputPin(inPinB);
    addOutputPin(outPinA);
}

QWidget* XProcessor::getConfigurationWidget() const
{
    return new QLabel("Configuration");
}

void XProcessor::refreshConfiguration()
{
}
