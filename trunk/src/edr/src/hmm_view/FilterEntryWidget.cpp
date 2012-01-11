#include "hmmPCH.h"
#include "FilterEntryWidget.h"
#include "HmmMainWindow.h"


void FilterEntryWidget::onButton()
{
    //hmm->clearTree();
    //QTreeWidgetItem* item = createTreeEntry(hmm->getCurrentSessions());
    //hmm->addItemToTree(item);
    if (!configurator) {
        configurator = filterCommand->getConfigurationWidget();
    }

    emit onFilterClicked(this);
}


QTreeWidgetItem* FilterEntryWidget::createTreeEntry(const std::vector<SessionConstPtr>& sessions)
{
    QTreeWidgetItem* item = filterCommand->createTreeBranch(getName(), sessions);
    item->setExpanded(true);
    return item;
}

