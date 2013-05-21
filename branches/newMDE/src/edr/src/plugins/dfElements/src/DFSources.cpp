#include "DFSources.h"
//##include "FilterCommand.h"
//#include "Vector3DFilterCommand.h"

//EMGSource::EMGSource( HmmMainWindow* hmm) :
//hmm(hmm)
//{
//    tree = new QTreeWidget();
//    tree->setHeaderHidden(true);
//    outPinA = new ScalarOutputPin(this);
//    addOutputPin(outPinA);
//    used = false;
//
//}
//
//EMGSource::~EMGSource()
//{
//    delete tree;
//}
//
//void EMGSource::reset()
//{
//    used = false;
//}
//
//const bool EMGSource::empty() const
//{
//    return used || !channel;
//}
//
//void EMGSource::produce()
//{
//    HmmTreeItem* treeItem = dynamic_cast<HmmTreeItem*>(tree->currentItem());
//    if (treeItem) {
//        TreeWrappedItemHelperPtr vectorItem = utils::dynamic_pointer_cast<TreeWrappedItemHelper>(treeItem->getHelper());
//        auto wrp = vectorItem->getWrapper();
//        channel = wrp->get();
//        outPinA->value(channel);
//        used = true;
//    } else {
//        throw std::runtime_error("Source is not set");
//    }
//}
//
//QWidget* EMGSource::getConfigurationWidget()
//{
//    return tree;
//}
//
//bool EMGSource::isNodeValid()
//{
//    return dynamic_cast<HmmTreeItem*>(tree->currentItem());
//}
//
//QString EMGSource::getErrorMessage()
//{
//    return QString("EMG source is not set");
//}
//
//void EMGSource::refreshConfiguration()
//{
//    tree->clear();
//    IFilterCommandPtr forceFilter(new BuilderFilterCommand(TreeBuilder::createEMGBranch, TreeBuilder::getRootEMGIcon(), TreeBuilder::getEMGIcon()));
//    QTreeWidgetItem* item = forceFilter->createTreeBranch(QObject::tr("Elements"), hmm->getCurrentSessions());
//    tree->addTopLevelItem(item);
//    tree->expandAll();
//}
