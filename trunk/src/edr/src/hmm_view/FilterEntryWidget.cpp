#include "hmmPCH.h"
#include "FilterEntryWidget.h"
#include "HmmMainWindow.h"


void FilterEntryWidget::onButton()
{
    hmm->clearTree();
    hmm->addItemToTree(filterCommand->createTreeBranch(getName(), hmm->getCurrentSessions()));
}

void FilterEntryWidget::onConfigurationButton()
{
    QWidget* configuration = filterCommand->getConfigurationWidget(this);
    if (configuration) {
        QDialog* dialog = new QDialog();
        dialog->setWindowFlags(Qt::Tool);
        dialog->setWindowTitle(getName());
        QHBoxLayout* layout = new QHBoxLayout();
        layout->addWidget(configuration);
        dialog->setLayout(layout);
        int w = configuration->width();
        int h = configuration->height();
        //dialog->resize(w, h);
        dialog->setFixedSize(w, h);
        dialog->exec();
        delete dialog;
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

void FilterEntryWidget::setActiveBackground( bool val )
{
    /*if (!val) {
    setStyleSheet("background-color: none");
    this->pushButton->setStyleSheet("background-color: none");
    } else {
    setStyleSheet("background-color: none");
    this->pushButton->setStyleSheet("background-color: none");
    }*/
}
