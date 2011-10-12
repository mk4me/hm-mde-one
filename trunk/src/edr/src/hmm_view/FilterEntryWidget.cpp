#include "FilterEntryWidget.h"
#include "HmmMainWindow.h"


void FilterEntryWidget::onButton()
{
    hmm->clearTree();
    hmm->addItemToTree(filterCommand->createTreeBranch(getName(), hmm->getCurrentSessions()));
}

void FilterEntryWidget::onConfigurationButton()
{
    QWidget* configuration = filterCommand->getConfigurationWidget();
    if (configuration) {
        QDialog* dialog = new QDialog(this);
        dialog->setWindowTitle(getName());
        QHBoxLayout* layout = new QHBoxLayout();
        layout->addWidget(configuration);
        dialog->setLayout(layout);
        dialog->exec();
    }
}
