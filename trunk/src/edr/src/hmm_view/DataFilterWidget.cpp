#include "DataFilterWidget.h"
#include "FilterEntryWidget.h"
#include "FilterCommand.h"


DataFilterWidget::DataFilterWidget( const QPixmap& pixmap, HmmMainWindow* hmmWindow ) :
    hmmWindow(hmmWindow),
    active(false)
{
    setupUi(this);
    this->pictureLabel->setPixmap(pixmap);
}

void DataFilterWidget::addFilter(const QString& bigLabelText, const QString& smallLabelText, DataFilterPtr dataFilter)
{
    SimpleFilterCommandPtr simple(new SimpleFilterCommand(dataFilter));
    addFilter(bigLabelText, smallLabelText, simple);
}

void DataFilterWidget::addFilter(FilterEntryWidget* entry) 
{
    entries.push_back(entry);
    this->verticalLayout->addWidget(entry);
}

void DataFilterWidget::addFilter( const QString& bigLabelText, const QString& smallLabelText, IFilterCommandPtr command )
{
    FilterEntryWidget* entry = new FilterEntryWidget(this, hmmWindow, bigLabelText, smallLabelText, command);
    addFilter(entry);
}

void DataFilterWidget::onClick()
{
    setActive(!getActive());
    const std::vector<SessionPtr>& sessions = hmmWindow->getCurrentSessions();
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
    if (val) {
        setStyleSheet("background-color: rgb(240, 240, 240)");
    } else {
        setStyleSheet("background-color: rgb(255, 255, 255)");
    }
    active = val;
    emit activated(val); 
}

