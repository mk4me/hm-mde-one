#include <plugins/dfElements/DFSinks.h>
#include <QtGui/QHBoxLayout>
#include <plugins/newVdf/TreeItemHelper.h>

void UniversalSink::_UniversalSink()
{
    static int count = 0;
    runNo = 0;
    inPinA = new UniversalInputPin(this);
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


void UniversalSink::consume()
{
    utils::ObjectWrapperConstPtr wrapper = inPinA->getWrapper();
    // HACK - trzeba by wprowadzic defaultowe helpery dla typow danych lub wprowadzic Vec3 do typow obslugiwanych przez NewChart
    core::HierarchyHelperPtr helper;
    if (wrapper->isSupported(typeid(VectorChannelReaderInterface))) {
        helper = core::HierarchyHelperPtr(new NewVector3ItemHelper(wrapper));
    } else {
        helper = core::HierarchyHelperPtr(new core::WrappedItemHelper(wrapper));
    }
    
    core::IHierarchyItemPtr dataItem = 
        core::HierarchyItemPtr(new core::HierarchyDataItem(wrapper, QIcon(), QString::fromStdString(wrapper->getClassName()), QString(), helper));
    emit itemConsumed(dataItem);
    this->dataItem = dataItem;
    //insertToTree(helper);//, wrapper);
    /*VectorChannelReaderInterfaceConstPtr c = inPinA->getVector();	
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
    }*/
}

void UniversalSink::reset()
{
    //runNo = hmm->getProcessedBranchCount();
}


UniversalSink::UniversalSink() 
{
    _UniversalSink();
}

QWidget* UniversalSink::getConfigurationWidget()
{
    return widget;    
}

/*
void UniversalSink::insertToTree( HierarchyHelperPtr channelHelper)//, utils::ObjectWrapperPtr wrp )
{
    HmmTreeItem* channelItem = new HmmTreeItem(channelHelper);
    QString name = line->text();
    if (name.isEmpty()) {
        name = defaultName;
    }
    channelItem->setItemAndHelperText(name);
    //wrp->insert(std::pair<std::string, std::string>("core/name", name.toStdString()));


    hmm->addItemToProcessedBranch(channelItem, runNo);
    hmm->switchToAnalysis();
}
*/

void UniversalInputPin::setWrapper( utils::ObjectWrapperConstPtr val )
{
    wrapper = val;
}

utils::ObjectWrapperConstPtr UniversalInputPin::getWrapper() const
{
    return wrapper;
}

const bool UniversalInputPin::pinCompatible( const df::IOutputPin * pin ) const
{
    return dynamic_cast<const vdf::IMDEOutputPin*>(pin);
}

void UniversalInputPin::copyData( const df::IDFOutput * pin )
{
    auto MDEPin = dynamic_cast<const vdf::IMDEOutputPin*>(pin);
    UTILS_ASSERT(MDEPin);
    wrapper = MDEPin->getWrapper();
}

void UniversalInputPin::reset()
{
    wrapper = utils::ObjectWrapperPtr();
}

UniversalInputPin::UniversalInputPin( df::ISinkNode * node ) : 
df::InputPin(node)
{

}
