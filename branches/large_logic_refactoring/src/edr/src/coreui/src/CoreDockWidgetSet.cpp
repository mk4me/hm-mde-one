#include "CoreUiPCH.h"
#include <coreui/CoreDockWidgetSet.h>
#include <coreui/CoreDockWidget.h>
#include <QtGui/QHBoxLayout>
#include <QtGui/QDockWidget>
#include <QtGui/QTabBar>

using namespace coreUI;

const int maxWidgets = 5;

CoreDockWidgetSet::CoreDockWidgetSet( const QString &title, QWidget *parent /*= 0*/, Qt::WindowFlags flags /*= 0*/ ) :
	QWidget(parent, flags),
	maxWidgetsNumber(maxWidgets),
	additionPossible(true)
{
	setWindowTitle(title);
	mainWindow = new QMainWindow();
    mainWindow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	mainWindow->setDockOptions(QMainWindow::AllowNestedDocks);
    mainWindow->setContentsMargins(7,7,7,7);
	auto layout = new QHBoxLayout;
	layout->addWidget(mainWindow);
    setLayout(layout);
}

CoreDockWidgetSet::CoreDockWidgetSet( QWidget *parent /*= nullptr*/, Qt::WindowFlags flags /*= 0*/ ) :
	QWidget(parent, flags),
	maxWidgetsNumber(maxWidgets), 
	additionPossible(true)
{
	mainWindow = new QMainWindow();
	mainWindow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	mainWindow->setDockOptions(QMainWindow::AllowNestedDocks);
    mainWindow->setContentsMargins(7,7,7,7);
	auto layout = new QHBoxLayout;
	layout->addWidget(mainWindow);
	setLayout(layout);
}

CoreDockWidgetSet::~CoreDockWidgetSet()
{

}

bool CoreDockWidgetSet::isAdditionPossible() const
{
	return (additionPossible && (getNumWidgets() < getMaxWidgetsNumber()));
}

void CoreDockWidgetSet::addDockWidget(QDockWidget* widget, Qt::DockWidgetArea area, Qt::Orientation orientation)
{
    QWidget::setUpdatesEnabled(false);
    if (isAdditionPossible() == true) {
        this->mainWindow->addDockWidget(area, widget, orientation);
        connect(widget, SIGNAL(destroyed(QObject*)), this, SLOT(onDockWidgetClosed(QObject*)));
        connect(widget, SIGNAL(dockLocationChanged ( Qt::DockWidgetArea )), this, SLOT(onDockWidgetLocationChanged(Qt::DockWidgetArea)));
        CoreDockWidget::setDockWidgetPermament(widget,false);
        widgetsList.push_back(widget);
    } else {
        throw std::runtime_error("Unable to add widget");
    }
    QWidget::setUpdatesEnabled(true);
    raise();
}

void CoreDockWidgetSet::addDockWidget( QDockWidget* widget, Qt::Orientation orientation )
{
	addDockWidget(widget, Qt::TopDockWidgetArea, orientation);
}

void CoreDockWidgetSet::addDockWidget( QDockWidget* widget )
{
    addDockWidget(widget, ((getNumWidgets() % 2) == 1) ? Qt::Horizontal : Qt::Vertical);
}

void CoreDockWidgetSet::removeDockWidget(QDockWidget * widget)
{
	auto it = std::find(widgetsList.begin(), widgetsList.end(), widget);
	if(it != widgetsList.end()){
		QWidget::setUpdatesEnabled(false);
		this->mainWindow->removeDockWidget(widget);
		disconnect(widget, SIGNAL(destroyed(QObject*)), this, SLOT(onDockWidgetClosed(QObject*)));
		disconnect(widget, SIGNAL(dockLocationChanged ( Qt::DockWidgetArea )), this, SLOT(onDockWidgetLocationChanged(Qt::DockWidgetArea)));
		widgetsList.erase(it);
		QWidget::setUpdatesEnabled(true);
	}
}

int CoreDockWidgetSet::getNumWidgets() const 
{
	return widgetsList.size();
}

void CoreDockWidgetSet::blockAddition( bool additionPossible )
{
	this->additionPossible = additionPossible;
}

void CoreDockWidgetSet::onDockWidgetClosed( QObject* object )
{ 
    Q_EMIT dockClosed();
    // konwersja wystarczająca, poniewaz chcemy tylko usunąć obiekt z listy
    QDockWidget* widget = reinterpret_cast<QDockWidget*>(object);
    widgetsList.remove(widget);
    if(widgetsList.empty() == true){
        close();
    }
}

void CoreDockWidgetSet::onDockWidgetLocationChanged(Qt::DockWidgetArea area)
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

int CoreDockWidgetSet::getMaxWidgetsNumber() const
{
	return maxWidgetsNumber;
}

void CoreDockWidgetSet::setMaxWidgetsNumber(int val)
{
	maxWidgetsNumber = val;
}

CoreDockWidgetSet::const_range CoreDockWidgetSet::getDockWidgets() const
{
	return boost::make_iterator_range(widgetsList.cbegin(), widgetsList.cend());
}


