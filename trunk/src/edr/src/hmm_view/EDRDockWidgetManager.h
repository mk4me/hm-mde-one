/********************************************************************
	created:	2011/09/20
	created:	20:9:2011   8:37
	filename: 	EDRDockWidgetManager.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMM__EDRDOCKWIDGETMANAGER_H__
#define HEADER_GUARD_HMM__EDRDOCKWIDGETMANAGER_H__
#include <QtGui/QWidget>
#include <QtGui/QMainWindow>

class EDRDockWidget;
class EDRDockWidgetSet;

class EDRDockWidgetManager : protected QMainWindow
{
public:
	explicit EDRDockWidgetManager(QWidget *parent = 0, Qt::WindowFlags flags = 0);
	virtual ~EDRDockWidgetManager() {}
	QWidget* asQWidget() { return dynamic_cast<QWidget*>(this); }

public:
	void addDockWidgetSet(EDRDockWidgetSet* set);
	void autoAddDockWidget(EDRDockWidget* widget);

private:
	std::list<EDRDockWidgetSet*> dockList;
	std::list<EDRDockWidgetSet*> generatedList;
};

#endif
