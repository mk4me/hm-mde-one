/********************************************************************
	created:	2013/04/09
	created:	9:4:2013   9:45
	filename: 	SimpleTab.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_MDE_VIEW__SIMPLETAB_H__
#define HEADER_GUARD_MDE_VIEW__SIMPLETAB_H__


#include "AbstractTab.h"

class SimpleTab : public AbstractTab
{
public:
    SimpleTab(QWidget* mainWidget, const QIcon& icon, const QString& label);
    SimpleTab(const QIcon& icon, const QString& label);
	virtual ~SimpleTab() {}

public:
    virtual QWidget* getMainWidget();
    virtual void registerConxtext( IAppUsageContextManager* manager, IAppUsageContextPtr parent, QTabWidget* flexiWidget );

private:
    QWidget* widget;
    IAppUsageContextPtr rootContext;
};
DEFINE_SMART_POINTERS(SimpleTab);

#endif
