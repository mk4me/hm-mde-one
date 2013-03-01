#include "hmmPCH.h"
#include "HmmSources.h"
#include "HmmMainWindow.h"

//void XSource::_XSource()
//{
//    outPinA = new XOutputPin(this);
//    addOutputPin(outPinA);
//    used = false;
//}
//
//void XSource::setChannel( VectorChannelReaderInterfaceConstPtr val )
//{
//    channel = val;
//}
//
//VectorChannelReaderInterfaceConstPtr XSource::getChannel() const
//{
//    return channel;
//}
//
//void XSource::produce()
//{
//    outPinA->value(channel);
//    used = true;
//}
//
//const bool XSource::empty() const
//{
//    return used;
//}
//
//void XSource::reset()
//{
//    used = false;
//}
//
//XSource::XSource( VectorChannelReaderInterfaceConstPtr vector ) :
//channel(vector)
//{
//    _XSource();
//}
//
//XSource::XSource()
//{
//    _XSource();
//}

QWidget* XSource::getConfigurationWidget() const
{
    return tree;
}

XSource::XSource()
{
    tree = new QTreeWidget();
    outPinA = new XOutputPin(this);
    addOutputPin(outPinA);
    used = false;
}

void XSource::reset()
{
    used = true;
}

const bool XSource::empty() const
{
    return used || !channel;
}

void XSource::produce()
{
    auto it = item2wrapper.find(tree->currentItem());
    if (it != item2wrapper.end()) {
        auto wrapper = it->second;
        channel = wrapper->get();
    }

    outPinA->value(channel);
    used = true;
}


void XSource::refreshConfiguration()
{
    tree->clear();
    item2wrapper.clear();
    core::ConstObjectsList vectors;
    plugin::getDataManagerReader()->getObjects(vectors, typeid(VectorChannelReaderInterface), false);


    for (auto it = vectors.begin(); it != vectors.end(); ++it) {
        QTreeWidgetItem* itm = new QTreeWidgetItem();
        auto wrapper = *it;
        item2wrapper[itm] = wrapper;
        auto desc = wrapper->find("core/name");
        if (desc != wrapper->end()) {
            itm->setText(0, desc->second.c_str());
        } else {
            itm->setText(0, wrapper->getClassName().c_str());
        }
        tree->addTopLevelItem(itm);
    }
    if (!vectors.empty()) {
        auto wrapper = *(vectors.begin());
        channel = wrapper->get();
    }
}
