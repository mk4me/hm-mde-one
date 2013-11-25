#include "NewVdfPCH.h"
#include <plugins/newVdf/UniversalSink.h>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLineEdit>
#include <plugins/newCommunication/TreeItemHelper.h>

using namespace vdf;

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

    core::IHierarchyItemPtr dataItem;

    if (wrapper->isSupported(typeid(core::ConstObjectsList))) {

        dataItem = core::HierarchyItemPtr(new core::HierarchyItem("Motion Analysis Results", QString()));

        core::shared_ptr<const core::ConstObjectsList> data;
        data = wrapper->get();

        for(auto it = data->begin(); it != data->end(); ++it){

            core::HierarchyHelperPtr helper;

            if ((*it)->isSupported(typeid(VectorChannelReaderInterface))) {
                helper = core::HierarchyHelperPtr(new NewVector3ItemHelper(*it));
            } else {
                helper = core::HierarchyHelperPtr(new core::WrappedItemHelper(*it));
            }

            std::string name = (*it)->getClassName();
            auto lIT = (*it)->find("core/label");
            if(lIT != (*it)->end()){
                name = lIT->second;
            }

            core::IHierarchyItemPtr di = 
                core::HierarchyItemPtr(new core::HierarchyDataItem(*it, QIcon(), QString::fromStdString(name), QString(), helper));

            dataItem->appendChild(di);
        }

    } else {

        core::HierarchyHelperPtr helper;

        if (wrapper->isSupported(typeid(VectorChannelReaderInterface))) {
            helper = core::HierarchyHelperPtr(new NewVector3ItemHelper(wrapper));
        } else {
            helper = core::HierarchyHelperPtr(new core::WrappedItemHelper(wrapper));
        }

        dataItem = core::HierarchyItemPtr(new core::HierarchyDataItem(wrapper, QIcon(), QString::fromStdString(wrapper->getClassName()), QString(), helper));
    }

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



