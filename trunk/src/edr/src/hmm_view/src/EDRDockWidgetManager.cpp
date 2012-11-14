#include "hmmPCH.h"
#include "EDRDockWidgetSet.h"
#include "EDRDockWidgetManager.h"
#include <QtGui/QTabWidget>

EDRDockWidgetManager::EDRDockWidgetManager( QWidget *parent /*= 0*/, Qt::WindowFlags flags /*= 0*/ ) :
	QMainWindow(parent, flags)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	setDockOptions(QMainWindow::ForceTabbedDocks | QMainWindow::AnimatedDocks | QMainWindow::AllowTabbedDocks);
    setCentralWidget(nullptr);
}

void EDRDockWidgetManager::addDockWidgetSet( EDRDockWidgetSet* set )
{
	this->addDockWidget(Qt::TopDockWidgetArea, set, Qt::Horizontal);
	dockList.push_back(set);

    connect(set, SIGNAL(dockClosed()), this, SLOT(dockClosed()));
	connect(set, SIGNAL(destroyed(QObject*)), this, SLOT(onSetClosed(QObject*)));
	if (dockList.size() > 1) {
		auto it = dockList.begin();
		auto it2 = dockList.begin();
		for (++it2; it2 != dockList.end(); ++it, ++it2) {
			this->tabifyDockWidget(*it, *it2);
		}

        //HACK!!
        //TO POWINNO SIĘ DAć STYLOWAć - NIESTETY JEST TO UKRYTE PRZEZ QT
        //TYLKO TAK MOŻNA USUNĄć ODSTĘP POMIĘDZY QTabBar a Widgetem związanym z aktualnym Tabem
        //patrz również: http://developer.qt.nokia.com/doc/qt-4.8/qstyleoptiontabbarbase.html
        // http://developer.qt.nokia.com/doc/qt-4.8/qstyleoptiontabbarbasev2.html#details
        // http://developer.qt.nokia.com/doc/qt-4.8/qstyle.html
        // http://developer.qt.nokia.com/doc/qt-4.8/qtabbar.html
        //

        QList<QTabBar*> tabBars = this->findChildren<QTabBar*>();

        for(int i = 0; i < tabBars.size(); ++i){
            QTabBar* tab = tabBars[i];
            if(tab != nullptr && tab->parent() == this){
                tab->setDrawBase(false);
            }
        }

        raiseSet(set);

    }
    set->setWindowTitle(QString(tr("Group %1")).arg(dockList.size()));
    set->setPermanent(false);
    //setTabNames();
}

void EDRDockWidgetManager::autoAddDockWidget( EDRDockWidget* widget )
{
	for (auto it = generatedList.begin(); it != generatedList.end(); ++it) {
		if ((*it)->isAdditionPossible(widget)) {
			(*it)->addDockWidget(widget);
			return;
		}
	}

	EDRDockWidgetSet* set = new EDRDockWidgetSet();
    connect(set, SIGNAL(dockClosed()), this, SLOT(dockClosed()));
	//if (set->isAdditionPossible(widget)) {
		set->addDockWidget(widget);
		addDockWidgetSet(set);
		generatedList.push_back(set);
	//} else {
		//throw std::runtime_error("Unable to add widget");
	//}
	//QWidget::setUpdatesEnabled(true);
    set->setPermanent(false);
    //setTabNames();
}


void EDRDockWidgetManager::onSetClosed( QObject* object )
{
    // edr dock widget już nie isnieje, ale potrzebny jest nam tylko wskaźnik,
    // zeby usunąć obiekt z listy
    EDRDockWidgetSet* set = reinterpret_cast<EDRDockWidgetSet*>(object);
    dockList.remove(set);
    generatedList.remove(set);
    setTabNames();
    Q_EMIT changed();
}

void EDRDockWidgetManager::setTabsPosition( QTabWidget::TabPosition tabPosition )
{
    setTabPosition(Qt::AllDockWidgetAreas, tabPosition);
}

void EDRDockWidgetManager::setTabNames()
{
    int i = 0;
    for (auto it = dockList.begin(); it != dockList.end(); ++it) {
        (*it)->setWindowTitle(QString(tr("Group %1")).arg(++i));
    }
}

void EDRDockWidgetManager::raiseSet( EDRDockWidgetSet* set )
{
    set->setVisible(true);
    set->setFocus();
    set->raise();
}

EDRDockWidgetSet* EDRDockWidgetManager::tryGetDockSet( EDRDockWidget* widget )
{
    for (auto it = dockList.begin(); it != dockList.end(); ++it) {
        BOOST_FOREACH(const EDRDockWidget* dock, (*it)->getDockWidgets()) {
            if (dock == widget) {
                return *it;
            }
        }

    }

    return nullptr;
}


void EDRDockWidgetManager::dockClosed()
{
    Q_EMIT changed();
}