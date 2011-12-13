#include "hmmPCH.h"
#include "FilterEntryWidget.h"
#include "HmmMainWindow.h"


void FilterEntryWidget::onButton()
{
    hmm->clearTree();
    QTreeWidgetItem* item = filterCommand->createTreeBranch(getName(), hmm->getCurrentSessions());
    item->setExpanded(true);
    hmm->addItemToTree(item);
}

void FilterEntryWidget::onConfigurationButton()
{
    if (!dialog) {
        dialog =  filterCommand->getConfigurationDialog(this);
    }
    if (dialog) {
        filterCommand->configurationStart();
        dialog->exec();
        int result = dialog->result();
        filterCommand->configurationStop(result ? IFilterCommand::OK : IFilterCommand::Cancel);
        if (result) {
            onButton();
        }
    }
}

void FilterEntryWidget::enterEvent( QEvent * )
{
    this->configurationButton->setVisible(true);
}

void FilterEntryWidget::leaveEvent( QEvent * )
{
    this->configurationButton->setVisible(false);
}

