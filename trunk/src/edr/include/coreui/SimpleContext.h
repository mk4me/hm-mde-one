/********************************************************************
	created:	2014/02/25
	created:	25:2:2014   11:09
	filename: 	SimpleContext.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_COREUI__SIMPLECONTEXT_H__
#define HEADER_GUARD_COREUI__SIMPLECONTEXT_H__

#include <coreui/IAppUsageContext.h>
#include <coreui/CoreFlexiToolBar.h>

class QTabWidget;

namespace coreUI {

    //! uniwersalny kontekst, sam rozmieszcza akcje
    class COREUI_EXPORT SimpleContext : public IAppUsageContext
    {
    public:
        //! konstruktor
        //! \param flexiTabWidget flexi widget kontekstu
        SimpleContext(QTabWidget * flexiTabWidget, const QString& name);

    public:
        //! wywo�ywane, po aktywowaniu si� kontekstu, do flexi bara trafi� akcje 
        //! \param contextWidget wiget, kt�ry wywo�a� kontekst
        virtual void activateContext(QWidget * contextWidget);
        //! wywo�ywane, gdy kontekst przestaje by� aktywny
        //! z flexibara znikaj� akcje 
        virtual void deactivateContext(QWidget * nextContextWidget, bool refresh);
        //! rejestracja kontekstu
        virtual void onRegisterContextWidget(QWidget * contextWidget);
        //! wyrejstrowanie kontekstu
        virtual void onUnregisterContextWidget(QWidget * contextWidget);


    private:
        //! flexi widget
        QTabWidget * flexiTabWidget;

        coreUI::CoreFlexiToolBar * widget;
        //! identyfikator grupy dla miniatur raport�w
        int groupID;
        //! nazwa dla glownej zak�adki flexi bara
        QString name;
    };
    DEFINE_SMART_POINTERS(SimpleContext);

}
#endif
