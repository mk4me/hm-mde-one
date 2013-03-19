#include "hmmPCH.h"
#include "HmmSinks.h"
#include "HmmMainWindow.h"

void XSink::_XSink()
{
    static int count = 0;
    runNo = 0;
    inPinA = new ChannelInputPin(this);
    addInputPin(inPinA);
    widget = new QWidget();
    QLayout* layout = new QHBoxLayout();
    widget->setLayout(layout);

    QLabel* label = new QLabel("Set name:");
    line = new QLineEdit();
    defaultName.sprintf("Sink: %04d", count++);
    line->setText(defaultName);
    layout->addWidget(label);
    layout->addWidget(line);
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
    VectorChannelReaderInterfaceConstPtr c = inPinA->getVector();	
    if (c) {
        utils::ObjectWrapperPtr wrp = utils::ObjectWrapper::create<VectorChannelReaderInterface>();
        wrp->set(core::const_pointer_cast<VectorChannelReaderInterface>(c));
        NewVector3ItemHelperPtr channelHelper(new NewVector3ItemHelper(wrp));
        insertToTree(channelHelper, wrp);
    } else {
        ScalarChannelReaderInterfaceConstPtr s = inPinA->getScalar();
        utils::ObjectWrapperPtr wrp = utils::ObjectWrapper::create<ScalarChannelReaderInterface>();
        wrp->set(core::const_pointer_cast<ScalarChannelReaderInterface>(s));
        NewChartItemHelperPtr channelHelper(new NewChartItemHelper(wrp));
        insertToTree(channelHelper, wrp);
    }
}

void XSink::reset()
{
    runNo = hmm->getProcessedBranchCount();
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

QWidget* XSink::getConfigurationWidget() const
{
    return widget;    
}

void XSink::refreshConfiguration()
{
    
}

void XSink::insertToTree( TreeItemHelperPtr channelHelper, utils::ObjectWrapperPtr wrp )
{
    HmmTreeItem* channelItem = new HmmTreeItem(channelHelper);
    QString name = line->text();
    if (name.isEmpty()) {
        name = defaultName;
    }
    channelItem->setItemAndHelperText(name);
    wrp->insert(std::pair<std::string, std::string>("core/name", name.toStdString()));


    hmm->addItemToProcessedBranch(channelItem, runNo);
    hmm->switchToAnalysis();
}
