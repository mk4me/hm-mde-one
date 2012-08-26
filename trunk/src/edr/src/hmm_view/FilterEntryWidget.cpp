#include "hmmPCH.h"
#include "FilterEntryWidget.h"
#include "HmmMainWindow.h"

FilterEntryWidget::FilterEntryWidget( QWidget* parent, HmmMainWindow* hmm,
    const QString& bigLabelText, IFilterCommandPtr filterCommand, const QPixmap* icon /*= nullptr*/ ) : 
        QWidget(parent),
        filterCommand(filterCommand),
        hmm(hmm),
        configurator(nullptr)
{
    setupUi(this);
    this->pushButton->setText(bigLabelText);
    this->pushButton->setAttribute(Qt::WA_NoSystemBackground, true);
    if (icon) {
        this->pushButton->setIcon(*icon);
    }
    connect(pushButton, SIGNAL(clicked()), this, SLOT(onButton()));
}

FilterEntryWidget::~FilterEntryWidget()
{
    if (configurator) {
        delete configurator;
    }
}

void FilterEntryWidget::onButton()
{
    if (!configurator) {
        configurator = filterCommand->getConfigurationWidget();
    }
    emit onFilterClicked(this);
}

QTreeWidgetItem* FilterEntryWidget::createTreeEntry(const std::vector<PluginSubject::SessionConstPtr>& sessions)
{
    QTreeWidgetItem* item = filterCommand->createTreeBranch(getName(), sessions);
    item->setExpanded(true);
    return item;
}

