#include <plugins/dfElements/DFSinks.h>
#include <QtGui/QHBoxLayout>
#include <plugins/newCommunication/TreeItemHelper.h>

void UniversalSink::_UniversalSink()
{
    static int count = 0;
    runNo = 0;
    inPinA = new UniversalInputPinBase(this);
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



