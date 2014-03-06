/********************************************************************
	created:	2013/04/09
	created:	9:4:2013   9:45
	filename: 	SimpleTab.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_MDE_VIEW__SIMPLETAB_H__
#define HEADER_GUARD_MDE_VIEW__SIMPLETAB_H__


#include <coreui/AbstractTab.h>

//! Podstawowa zak³adka g³ównego okna MDE.
//! W konstruktorze pobiera widget, na podstawie dostarczonych przez niego akcji tworzony jest kontekst 
class SimpleTab : public coreUI::AbstractTab
{
public:
    SimpleTab(QWidget* mainWidget, const QIcon& icon, const QString& label);
    SimpleTab(const QIcon& icon, const QString& label);
	virtual ~SimpleTab() {}

public:
    //! \return dostarczony w konstruktorze widget
    virtual QWidget* getMainWidget();
    //! \return podpina kontekst zak³adki, jeœli dostarzony widget ma akcje, to metoda je automatycznie rozmiesci w kontekœcie.
    virtual void registerConxtext( coreUI::IAppUsageContextManager* manager, coreUI::IAppUsageContextPtr parent, QTabWidget* flexiWidget );

private:
    QWidget* widget;
    coreUI::IAppUsageContextPtr rootContext;
};
DEFINE_SMART_POINTERS(SimpleTab);

#endif
