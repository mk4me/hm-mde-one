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
    mainWindow->setContentsMargins(7,7,7,7);
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
    mainWindow->setContentsMargins(7,7,7,7);
    setWidget(mainWindow);
}

bool EDRDockWidgetSet::isAdditionPossible(EDRDockWidget* widget) const
{
	return (additionPossible && (getNumWidgets() < getMaxWidgetsNumber()));
}

void EDRDockWidgetSet::addDockWidget(EDRDockWidget* widget, Qt::DockWidgetArea area, Qt::Orientation orientation)
{
    QWidget::setUpdatesEnabled(false);
    if (isAdditionPossible(widget)) {
        this->mainWindow->addDockWidget(area, widget, orientation);
        connect(widget, SIGNAL(destroyed(QObject*)), this, SLOT(onDockWidgetClosed(QObject*)));
        connect(widget, SIGNAL(dockLocationChanged ( Qt::DockWidgetArea )), this, SLOT(onDockWidgetLocationChanged(Qt::DockWidgetArea)));
        widget->setPermanent(false);
        widgetsList.push_back(widget);
    } else {
        throw std::runtime_error("Unable to add widget");
    }
    QWidget::setUpdatesEnabled(true);
    raise();
}

void EDRDockWidgetSet::addDockWidget( EDRDockWidget* widget, Qt::Orientation orientation )
{
	addDockWidget(widget, Qt::TopDockWidgetArea, orientation);
}

void EDRDockWidgetSet::addDockWidget( EDRDockWidget* widget )
{
    addDockWidget(widget, ((getNumWidgets() % 2) == 1) ? Qt::Horizontal : Qt::Vertical);
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
    emit dockClosed();
    // konwersja wystarczająca, poniewaz chcemy tylko usunąć obiekt z listy
    EDRDockWidget* widget = reinterpret_cast<EDRDockWidget*>(object);
    widgetsList.remove(widget);
    if(widgetsList.empty() == true){
        close();
    }
}

void EDRDockWidgetSet::onDockWidgetLocationChanged(Qt::DockWidgetArea area)
{
    //HACK!!
    //TO POWINNO SIĘ DAć STYLOWAć - NIESTETY JEST TO UKRYTE PRZEZ QT
    //TYLKO TAK MOŻNA USUNĄć ODSTĘP POMIĘDZY QTabBar a Widgetem związanym z aktualnym Tabem
    //patrz również: http://developer.qt.nokia.com/doc/qt-4.8/qstyleoptiontabbarbase.html
    // http://developer.qt.nokia.com/doc/qt-4.8/qstyleoptiontabbarbasev2.html#details
    // http://developer.qt.nokia.com/doc/qt-4.8/qstyle.html
    // http://developer.qt.nokia.com/doc/qt-4.8/qtabbar.html
    // 
    QDockWidget * changedDock = qobject_cast<QDockWidget*>(sender());    
    QList<QTabBar*> tabBars = mainWindow->findChildren<QTabBar*>();
    for(int i = 0; i < tabBars.size(); ++i){
        QTabBar* tab = tabBars[i];
        if(tab->parent() == mainWindow){
            tab->setDrawBase(false);
        }
    }

    //odswieżamy tytuł okna - konieczne by style tabów poprawnie działałay (text się dobrze renderował)
    if(changedDock->windowTitle().isEmpty() == false){
        QString originalTitle(changedDock->windowTitle());
        changedDock->setWindowTitle(originalTitle + QString(" "));
        changedDock->setWindowTitle(originalTitle);
    }
}


