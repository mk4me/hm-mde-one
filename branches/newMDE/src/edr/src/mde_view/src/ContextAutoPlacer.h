/********************************************************************
	created:	2013/04/23
	created:	23:4:2013   23:11
	filename: 	ContextAutoPlacer.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_MDE_VIEW__CONTEXTAUTOPLACER_H__
#define HEADER_GUARD_MDE_VIEW__CONTEXTAUTOPLACER_H__

#include <QtGui/QWidget>
#include <QtCore/QList>
#include <QtGui/QAction>
#include <QtGui/QTabWidget>
#include <coreui/CoreFlexiToolBar.h>

class ContextAutoPlacer
{
public:
    static QWidget* create(const QList<QAction*>& actions);
};

//! uniwersalny kontekst, sam rozmieszcza akcje
class SimpleContext : public IAppUsageContext
{
public:
    //! konstruktor
    //! \param flexiTabWidget flexi widget kontekstu
    SimpleContext(QTabWidget * flexiTabWidget, const QString& name);

public:
    //! wywoływane, po aktywowaniu się kontekstu, do flexi bara trafią akcje 
    //! \param contextWidget wiget, który wywołał kontekst
    virtual void activateContext(QWidget * contextWidget);
    //! wywoływane, gdy kontekst przestaje być aktywny
    //! z flexibara znikają akcje 
    virtual void deactivateContext(QWidget * nextContextWidget, bool refresh);
    //! rejestracja kontekstu
    virtual void onRegisterContextWidget(QWidget * contextWidget);
    //! wyrejstrowanie kontekstu
    virtual void onUnregisterContextWidget(QWidget * contextWidget);


private:
    //! flexi widget
    QTabWidget * flexiTabWidget;
    
    coreUI::CoreFlexiToolBar * widget;
    //! identyfikator grupy dla miniatur raportów
    int groupID;
    //! nazwa dla glownej zakładki flexi bara
    QString name;
};
DEFINE_SMART_POINTERS(SimpleContext);



#endif
