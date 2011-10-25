#include "hmmPCH.h"
#include "DataFilterWidget.h"
#include "FilterEntryWidget.h"
#include "FilterCommand.h"


DataFilterWidget::DataFilterWidget(const QString& name, const QPixmap& pixmap, HmmMainWindow* hmmWindow) :
    hmmWindow(hmmWindow),
    active(false),
    filtersClosed(false)
{
    setupUi(this);
    this->label->setText(name);
    this->pictureLabel->setPixmap(pixmap);
    //this->verticalLayout->setAlignment(Qt::AlignTop);
}

void DataFilterWidget::addFilter(const QString& bigLabelText, const QString& smallLabelText, DataFilterPtr dataFilter, const QPixmap* icon)
{
    SimpleFilterCommandPtr simple(new SimpleFilterCommand(dataFilter));
    addFilter(bigLabelText, smallLabelText, simple, icon);
}

void DataFilterWidget::addFilter(FilterEntryWidget* entry) 
{
    UTILS_ASSERT(filtersClosed == false);
    entries.push_back(entry);
    this->verticalLayout->addWidget(entry);
}

void DataFilterWidget::addFilter( const QString& bigLabelText, const QString& smallLabelText, IFilterCommandPtr command, const QPixmap* icon)
{
    FilterEntryWidget* entry = new FilterEntryWidget(this, hmmWindow, bigLabelText, smallLabelText, command, icon);
    addFilter(entry);
}

void DataFilterWidget::onClick()
{
    setActive(!getActive());
    const std::vector<SessionConstPtr>& sessions = hmmWindow->getCurrentSessions();
    hmmWindow->clearTree();
    BOOST_FOREACH(FilterEntryWidget* filter, entries) {
        hmmWindow->addItemToTree(filter->getFilterCommand()->createTreeBranch(filter->getName(), sessions));
    }
}

void DataFilterWidget::mousePressEvent( QMouseEvent *e )
{
    if (e->button() == Qt::LeftButton) {
        onClick();
    } else {
        QWidget::mousePressEvent(e);
    }
}

void DataFilterWidget::setActive( bool val )
{
    if (!val) {
        setStyleSheet("background-color: rgb(235, 235, 235)");
    } else {
        setStyleSheet("background-color: rgb(255, 255, 255)");
    }

    for (auto it = entries.begin(); it != entries.end(); it++) {
        (*it)->setActiveBackground(val);
    }

    active = val;
    emit activated(val); 
}

void DataFilterWidget::closeFilters()
{
    UTILS_ASSERT(filtersClosed == false && entries.size() > 0 && entries.size() <= 3);
    filtersClosed = true;
    FilterEntryWidget* e = entries[0];
    QSpacerItem* item = new QSpacerItem(0, e->height() * (3 - entries.size()));
    verticalLayout->addSpacerItem(item);
}

