#include "EdrDockWidgetSet.h"
#include "EDRDockWidgetManager.h"


EDRDockWidgetManager::EDRDockWidgetManager( QWidget *parent /*= 0*/, Qt::WindowFlags flags /*= 0*/ ) :
	QMainWindow(parent, flags)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	setDockOptions(QMainWindow::ForceTabbedDocks);
}


void EDRDockWidgetManager::addDockWidgetSet( EDRDockWidgetSet* set )
{
	QWidget::setUpdatesEnabled(false);

	this->addDockWidget(Qt::TopDockWidgetArea, set, Qt::Horizontal);
	dockList.push_back(set);
	 
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

}
