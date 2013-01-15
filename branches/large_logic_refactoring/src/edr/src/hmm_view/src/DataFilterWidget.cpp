#include "hmmPCH.h"
#include "DataFilterWidget.h"
#include "FilterEntryWidget.h"
#include "FilterCommand.h"

using namespace PluginSubject;

DataFilterWidget::DataFilterWidget(const QString& name, const QPixmap& pixmap, HmmMainWindow* hmmWindow) :
    hmmWindow(hmmWindow),
    active(false),
    filtersClosed(false)
{
    setupUi(this);
    setActive(false);
    this->label->setText(name);
    this->pictureLabel->setPixmap(pixmap);
    this->installEventFilter(this);
    colorBox->installEventFilter(this);
}

bool DataFilterWidget::eventFilter(QObject *object, QEvent *event)
{
    if ( event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            onClick();
            return true;
        } else {
            return false;
        }
    }
    return false;
}

void DataFilterWidget::addFilter(const QString& bigLabelText, DataFilterPtr dataFilter, const QPixmap* icon)
{
    SimpleFilterCommandPtr simple(new SimpleFilterCommand(dataFilter));
    addFilter(bigLabelText, simple, icon);
}

void DataFilterWidget::addFilter(FilterEntryWidget* entry) 
{
    UTILS_ASSERT(filtersClosed == false);
    entries.push_back(entry);
    this->verticalLayout->addWidget(entry);
}

void DataFilterWidget::addFilter( const QString& bigLabelText, IFilterCommandPtr command, const QPixmap* icon)
{
    FilterEntryWidget* entry = new FilterEntryWidget(this, hmmWindow, bigLabelText, command, icon);
    addFilter(entry);
}

void DataFilterWidget::onClick()
{
    uncheckEntries();
    const std::vector<PluginSubject::SessionConstPtr>& sessions = hmmWindow->getCurrentSessions();
    hmmWindow->clearTree();
    BOOST_FOREACH(FilterEntryWidget* filter, entries) {
        hmmWindow->addItemToTree(filter->getFilterCommand()->createTreeBranch(filter->getName(), sessions));
    } 
    setActive(!getActive());
    Q_EMIT clicked();
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
    setColor(val ? QColor(235, 235, 235) : Qt::white);
    active = val;
    Q_EMIT activated(val); 
}

void DataFilterWidget::closeFilters()
{
    UTILS_ASSERT(filtersClosed == false && entries.empty() == false && entries.size() <= 3);
    filtersClosed = true;
    FilterEntryWidget* e = entries[0];
    QSpacerItem* item = new QSpacerItem(0, e->height() * (3 - entries.size()));
    verticalLayout->addSpacerItem(item);
}

void DataFilterWidget::setColor( const QColor& color )
{
    QString style = QString("#colorBox { background: rgb(%1, %2, %3); }").arg(color.red()).arg(color.green()).arg(color.blue());
    colorBox->setStyleSheet(style);
}

const FilterEntryWidget* DataFilterWidget::getEntry( int index ) const
{
    UTILS_ASSERT(index >= 0 && index < static_cast<int>(entries.size()));
    return entries[index];
}

void DataFilterWidget::uncheckEntries( FilterEntryWidget* toSkip)
{
    for (auto it = entries.begin(); it != entries.end(); ++it) {
        if (toSkip != *it) {
            (*it)->blockSignals(true);
            (*it)->setChecked(false);
            (*it)->blockSignals(false);
        }
    }
}

void DataFilterWidget::resetFilters()
{
    blockSignals(true);
    setActive(false);
    uncheckEntries();
    for (auto it = entries.begin(); it != entries.end(); ++it) {
        (*it)->getFilterCommand()->reset();
    }
    blockSignals(false);
}


