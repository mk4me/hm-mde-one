#include "hmmPCH.h"
#include "EdrDockWidgetSet.h"
#include "EDRDockWidgetManager.h"


EDRDockWidgetManager::EDRDockWidgetManager( QWidget *parent /*= 0*/, Qt::WindowFlags flags /*= 0*/ ) :
	QMainWindow(parent, flags)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	setDockOptions(QMainWindow::ForceTabbedDocks);
    //setDocumentMode(true);
    setDocumentMode(true);
}


void EDRDockWidgetManager::addDockWidgetSet( EDRDockWidgetSet* set )
{
	QWidget::setUpdatesEnabled(false);
    //setDocumentMode(true);
    setDocumentMode(true);
    set->setWindowTitle(QString(tr("Tab %1")).arg(dockList.size()));
	this->addDockWidget(Qt::TopDockWidgetArea, set, Qt::Horizontal);
	dockList.push_back(set);
    
	connect(set, SIGNAL(destroyed(QObject*)), this, SLOT(onSetClosed(QObject*)));
	if (dockList.size() > 1) {
		auto it = dockList.begin();
		auto it2 = dockList.begin();
		for (++it2; it2 != dockList.end(); it++, it2++) {
			this->tabifyDockWidget(*it, *it2);
		}
		set->setVisible(true);
		set->setFocus();
		set->raise();
	} 
	QWidget::setUpdatesEnabled(true);
    set->setPermanent(false);
}

void EDRDockWidgetManager::autoAddDockWidget( EDRDockWidget* widget )
{
    QWidget::setUpdatesEnabled(false);
	for (auto it = generatedList.begin(); it != generatedList.end(); it++) {
		if ((*it)->isAdditionPossible(widget)) {
			(*it)->addDockWidget(widget);
			QWidget::setUpdatesEnabled(true);
			return;
		}
	}

	EDRDockWidgetSet* set = new EDRDockWidgetSet(this);
	if (set->isAdditionPossible(widget)) {
		set->addDockWidget(widget);
		addDockWidgetSet(set);
		generatedList.push_back(set);
	} else {
		throw std::runtime_error("Unable to add widget");
	}
	QWidget::setUpdatesEnabled(true);
    set->setPermanent(false);
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
        (*it)->setWindowTitle(QString(tr("Tab %1")).arg(++i));
    }
}
