#include "hmmPCH.h"
#include "EdrDockWidgetSet.h"
#include "EDRDockWidgetManager.h"
#include <QtGui/QTabWidget>

EDRDockWidgetManager::EDRDockWidgetManager( QWidget *parent /*= 0*/, Qt::WindowFlags flags /*= 0*/ ) :
	QMainWindow(parent, flags)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	setDockOptions(QMainWindow::ForceTabbedDocks | QMainWindow::AnimatedDocks | QMainWindow::AllowTabbedDocks);
    setCentralWidget(nullptr);
    //setDocumentMode(true);
}

void EDRDockWidgetManager::addDockWidgetSet( EDRDockWidgetSet* set )
{
	//QWidget::setUpdatesEnabled(false);
    //setDocumentMode(true);
	this->addDockWidget(Qt::TopDockWidgetArea, set, Qt::Horizontal);
	dockList.push_back(set);

	connect(set, SIGNAL(destroyed(QObject*)), this, SLOT(onSetClosed(QObject*)));
	if (dockList.size() > 1) {
		auto it = dockList.begin();
		auto it2 = dockList.begin();
		for (++it2; it2 != dockList.end(); it++, it2++) {
			this->tabifyDockWidget(*it, *it2);
		}

        //HACK!!
        //TO POWINNO SIÊ DAæ STYLOWAæ - NIESTETY JEST TO UKRYTE PRZEZ QT
        //TYLKO TAK MO¯NA USUN¥æ ODSTÊP POMIÊDZY QTabBar a Widgetem zwi¹zanym z aktualnym Tabem
        //patrz równie¿: http://developer.qt.nokia.com/doc/qt-4.8/qstyleoptiontabbarbase.html
        // http://developer.qt.nokia.com/doc/qt-4.8/qstyleoptiontabbarbasev2.html#details
        // http://developer.qt.nokia.com/doc/qt-4.8/qstyle.html
        // http://developer.qt.nokia.com/doc/qt-4.8/qtabbar.html
        // 

        QList<QTabBar*> tabBars = this->findChildren<QTabBar*>();

        for(int i = 0; i < tabBars.size(); i++){
            QTabBar* tab = tabBars[i];
            if(tab != nullptr && tab->parent() == this){
                tab->setDrawBase(false);
            }
        }

        raiseSet(set);

    }

    set->setWindowTitle(QString(tr("Group %1")).arg(dockList.size()));

	//QWidget::setUpdatesEnabled(true);
    set->setPermanent(false);
    //setTabNames();
}

void EDRDockWidgetManager::autoAddDockWidget( EDRDockWidget* widget )
{
    //QWidget::setUpdatesEnabled(false);
	for (auto it = generatedList.begin(); it != generatedList.end(); it++) {
		if ((*it)->isAdditionPossible(widget)) {
			(*it)->addDockWidget(widget);
			//QWidget::setUpdatesEnabled(true);
			return;
		}
	}

	EDRDockWidgetSet* set = new EDRDockWidgetSet();
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
    // edr dock widget juz nie isnieje, ale potrzebny jest nam tylko wskaznik,
    // zeby usunac obiekt z listy
    EDRDockWidgetSet* set = reinterpret_cast<EDRDockWidgetSet*>(object);
    dockList.remove(set);
    generatedList.remove(set);
    setTabNames();
}

void EDRDockWidgetManager::setTabsPosition( QTabWidget::TabPosition tabPosition )
{
    setTabPosition(Qt::AllDockWidgetAreas, tabPosition);
}

void EDRDockWidgetManager::setTabNames()
{
    int i = 0;
    for (auto it = dockList.begin(); it != dockList.end(); it++) {
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
    for (auto it = dockList.begin(); it != dockList.end(); it++) {
        BOOST_FOREACH(const EDRDockWidget* dock, (*it)->getDockWidgets()) {
            if (dock == widget) {
                return *it;
            }
        }
        
    }

    return nullptr;
}
