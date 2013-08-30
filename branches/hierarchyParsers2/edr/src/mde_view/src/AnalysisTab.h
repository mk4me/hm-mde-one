/********************************************************************
	created:	2013/04/24
	created:	24:4:2013   16:15
	filename: 	AnalysisTab.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_MDE_VIEW__ANALYSISTAB_H__
#define HEADER_GUARD_MDE_VIEW__ANALYSISTAB_H__

#include "AbstractTab.h"
#include <coreui/CoreFlexiToolBar.h>
#include <boost/tuple/tuple.hpp>
#include <QtGui/QTabWidget>
#include "AnalisisWidget.h"

class AnalysisTab : public AbstractTab
{
public:
    AnalysisTab(AnalisisWidget* widget, const QIcon& icon, const QString& label);
	virtual ~AnalysisTab() {}

public:
    virtual QWidget* getMainWidget();
    virtual void registerConxtext( IAppUsageContextManager* manager, IAppUsageContextPtr parent, QTabWidget * flexiTabWidget ) ;

private:
    AnalisisWidget* widget;
};
DEFINE_SMART_POINTERS(AnalysisTab);

#endif
