#include "hmmPCH.h"
#include "EDRDockWidgetSet.h"
#include "EDRTitleBar.h"

const int maxWidgets = 5;


EDRDockWidgetSet::EDRDockWidgetSet( const QString &title, QWidget *parent /*= 0*/, Qt::WindowFlags flags /*= 0*/ ) :
	EDRDockWidget(title, parent, flags),
	maxWidgetsNumber(maxWidgets),
	additionPossible(true)
{
    supplyWithEDRTitleBar(this);

	mainWindow = new QMainWindow();
    mainWindow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //mainWindow->setDocumentMode(true);
    setWidget(mainWindow);
}

EDRDockWidgetSet::EDRDockWidgetSet( QWidget *parent /*= nullptr*/, Qt::WindowFlags flags /*= 0*/ ) :
	EDRDockWidget(parent, flags),
	maxWidgetsNumber(maxWidgets), 
	additionPossible(true)
{
    supplyWithEDRTitleBar(this);

	mainWindow = new QMainWindow();
	mainWindow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //mainWindow->setDocumentMode(true);
    setWidget(mainWindow);
}

bool EDRDockWidgetSet::isAdditionPossible(EDRDockWidget* widget) const
{
	return (additionPossible && (getNumWidgets() < getMaxWidgetsNumber()));
}

void EDRDockWidgetSet::addDockWidget( EDRDockWidget* widget, Qt::Orientation orientation )
{
	QWidget::setUpdatesEnabled(false);
	if (isAdditionPossible(widget)) {
		this->mainWindow->addDockWidget(Qt::TopDockWidgetArea, widget, orientation);
        connect(widget, SIGNAL(destroyed(QObject*)), this, SLOT(onDockWidgetClosed(QObject*)));
        widget->setPermanent(false);
		widgetsList.push_back(widget);
	} else {
		throw std::runtime_error("Unable to add widget");
	}
	QWidget::setUpdatesEnabled(true);
    raise();
}

void EDRDockWidgetSet::addDockWidget( EDRDockWidget* widget )
{
    addDockWidget(widget, getNumWidgets() % 2 ? Qt::Horizontal : Qt::Vertical);
}

int EDRDockWidgetSet::getNumWidgets() const 
{
	return widgetsList.size();
}

void EDRDockWidgetSet::blockAddition( bool additionPossible )
{
	this->additionPossible = additionPossible;
}

void EDRDockWidgetSet::onDockWidgetClosed( QObject* object )
{
    // konwersja wystarczajaca, poniewaz chcemy tylko usunac obiekt z listy
    EDRDockWidget* widget = reinterpret_cast<EDRDockWidget*>(object);
    widgetsList.remove(widget);
}


