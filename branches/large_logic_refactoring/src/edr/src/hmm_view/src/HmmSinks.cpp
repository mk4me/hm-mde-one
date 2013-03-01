#include "hmmPCH.h"
#include "HmmSinks.h"
#include "HmmMainWindow.h"

void XSink::_XSink()
{
    inPinA = new XInputPin(this);
    addInputPin(inPinA);
}

void XSink::setHmm( HmmMainWindow* val )
{
    hmm = val;
}

HmmMainWindow* XSink::getHmm() const
{
    return hmm;
}

void XSink::consume()
{
    VectorChannelReaderInterfaceConstPtr c = inPinA->value();	
    if (c) {
        utils::ObjectWrapperPtr wrp = utils::ObjectWrapper::create<VectorChannelReaderInterface>();
        wrp->set(core::const_pointer_cast<VectorChannelReaderInterface>(c));
        NewVector3ItemHelperPtr channelHelper(new NewVector3ItemHelper(wrp));

        HmmTreeItem* channelItem = new HmmTreeItem(channelHelper);
        channelItem->setItemAndHelperText(QString::fromStdString("CREATED!"));
        hmm->addItemToTree(channelItem);
    }
}

void XSink::reset()
{

}

XSink::XSink( HmmMainWindow* h ) : 
hmm(h)
{
    _XSink();
}

XSink::XSink() : 
hmm(nullptr)
{
    _XSink();
}
