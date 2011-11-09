/********************************************************************
	created:	2011/09/19
	created:	19:9:2011   14:55
	filename: 	EDRDockWidgetSet.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMM__EDRDOCKWIDGETSET_H__
#define HEADER_GUARD_HMM__EDRDOCKWIDGETSET_H__

#include <QtCore/QString>
#include <QtGui/QMainWindow>
#include "EDRDockWidget.h"
#include "EDRDockInnerWidget.h"


class EDRDockWidgetSet : public EDRDockWidget
{
	Q_OBJECT;
public:
	explicit EDRDockWidgetSet(const QString &title, QWidget *parent = 0, Qt::WindowFlags flags = 0);
	explicit EDRDockWidgetSet(QWidget *parent = nullptr, Qt::WindowFlags flags = 0);
	virtual ~EDRDockWidgetSet() {}

public:
	void addDockWidget(EDRDockWidget* widget, Qt::Orientation orientation);
    void addDockWidget(EDRDockWidget* widget);
	bool isAdditionPossible(EDRDockWidget* widget) const;
	void blockAddition(bool additionPossible);
	int getNumWidgets() const;
	int getMaxWidgetsNumber() const { return maxWidgetsNumber; }
	void setMaxWidgetsNumber(int val) { maxWidgetsNumber = val; }

private:
	QMainWindow* mainWindow;
	std::list<EDRDockWidget*> widgetsList;
	int maxWidgetsNumber;
	bool additionPossible;
};

#endif
