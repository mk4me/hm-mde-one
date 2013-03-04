#include "hmmPCH.h"
#include "HmmSources.h"
#include "HmmMainWindow.h"
#include "FilterCommand.h"
#include "Vector3DFilterCommand.h"



QWidget* XSource::getConfigurationWidget() const
{
    return tree;
}

XSource::XSource(HmmMainWindow* hmm, BuilderFilterCommand::BranchFunction fun, const QIcon& rootIcon, const QIcon& leafIcon) :
    branchFunction(fun),
    rootIcon(rootIcon),
    leafIcon(leafIcon),
    hmm(hmm)
{
    tree = new QTreeWidget();
    tree->setHeaderHidden(true);
    outPinA = new VectorOutputPin(this);
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
    HmmTreeItem* treeItem = dynamic_cast<HmmTreeItem*>(tree->currentItem());
    if (treeItem) {
        NewVector3ItemHelperPtr vectorItem = core::dynamic_pointer_cast<NewVector3ItemHelper>(treeItem->getHelper());
        auto wrp = vectorItem->getWrapper();
        channel = wrp->get();
        outPinA->value(channel);
        used = true;
    } else {
        throw std::runtime_error("Source is not set");
    }
}


void XSource::refreshConfiguration()
{
    tree->clear();
    IFilterCommandPtr forceFilter(new BuilderFilterCommand(branchFunction, rootIcon, leafIcon));
    QTreeWidgetItem* item = forceFilter->createTreeBranch(QObject::tr("Elements"), hmm->getCurrentSessions());
    tree->addTopLevelItem(item);
    tree->expandAll();
}

XSource::~XSource()
{
    delete tree;
}

bool XSource::isNodeValid()
{
    return dynamic_cast<HmmTreeItem*>(tree->currentItem());
}

QString XSource::getErrorMessage()
{
    return QString("Source is not set");
}
