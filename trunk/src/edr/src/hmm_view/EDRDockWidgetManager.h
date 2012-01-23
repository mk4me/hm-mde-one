/********************************************************************
	created:	2011/09/20
	created:	20:9:2011   8:37
	filename: 	EDRDockWidgetManager.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMM__EDRDOCKWIDGETMANAGER_H__
#define HEADER_GUARD_HMM__EDRDOCKWIDGETMANAGER_H__

#include <boost/range.hpp>
#include <QtGui/QWidget>
#include <QtGui/QMainWindow>

class EDRDockWidget;
class EDRDockWidgetSet;

class EDRDockWidgetManager : public QMainWindow
{
    Q_OBJECT;

public:
    typedef boost::iterator_range<std::list<EDRDockWidgetSet*>::iterator> range;
    typedef boost::iterator_range<std::list<EDRDockWidgetSet*>::const_iterator> const_range;
public:
	explicit EDRDockWidgetManager(QWidget *parent = 0, Qt::WindowFlags flags = 0);
	virtual ~EDRDockWidgetManager() {}
	QWidget* asQWidget() { return dynamic_cast<QWidget*>(this); }

public:
	void addDockWidgetSet(EDRDockWidgetSet* set);

    void raiseSet( EDRDockWidgetSet* set );

    void autoAddDockWidget(EDRDockWidget* widget);
    void setTabsPosition(QTabWidget::TabPosition tabPosition);
    const_range getDockSet() const { return boost::make_iterator_range(dockList.cbegin(), dockList.cend()); }
    range getDockSet() { return boost::make_iterator_range(dockList.begin(), dockList.end()); }
    EDRDockWidgetSet* tryGetDockSet(EDRDockWidget* widget);

private:
	std::list<EDRDockWidgetSet*> dockList;
	std::list<EDRDockWidgetSet*> generatedList;

private:
    void setTabNames();

private slots:
    void onSetClosed(QObject* object);
};

#endif
