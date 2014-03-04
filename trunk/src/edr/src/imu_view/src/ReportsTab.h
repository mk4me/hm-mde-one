/********************************************************************
	created:	2013/04/15
	created:	15:4:2013   9:06
	filename: 	ReportsTab.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_MDE__REPORTSTAB_H__
#define HEADER_GUARD_MDE__REPORTSTAB_H__

#include "AbstractTab.h"
#include <coreui/CoreFlexiToolBar.h>
#include <boost/tuple/tuple.hpp>
#include <QtGui/QTabWidget>

//! kontekst dla zak�adki z raportami
class ReportsTabContext : public QObject, public coreUI::IAppUsageContext
{
    Q_OBJECT;
public:
    //! konstruktor
    //! \param flexiTabWidget flexi widget kontekstu
    //! \param hmm g��wne okno aplikacji
    ReportsTabContext(QTabWidget * flexiTabWidget);

public:
    //! wywo�ywane, po aktywowaniu si� kontekstu, do flexi bara trafi� akcje zwi�zane z oknem raport�w
    //! \param contextWidget wiget, kt�ry wywo�a� kontekst
    virtual void activateContext(QWidget * contextWidget);
    //! wywo�ywane, gdy kontekst przestaje by� aktywny
    //! z flexibara znikaj� akcje zwi�zane z oknem raport�w
    virtual void deactivateContext(QWidget * nextContextWidget, bool refresh);
    //! rejestracja okna raport�w
    virtual void onRegisterContextWidget(QWidget * contextWidget);
    //! wyrejstrowanie okna raport�w
    virtual void onUnregisterContextWidget(QWidget * contextWidget);

private:
    //! tworzy zbiorczy widget, kt�ry jest kontenerem dla dw�ch wierszy widget�w potomnych
    //! \return krotka z utworzonym widgetem oraz layoutami dla wiersza g�rnego i dolnego
    boost::tuple<QWidget*, QLayout*, QLayout*> createTwoLineWidget();
    //! automatycznie rozmieszcza widgety w dw�ch wierszach
    //! \param editList lista z akcjami i obiektami
    //! \param lowerLayout dolny wiersz
    //! \param upperLayout g�rny wiersz
    //! \param actionsOnTop czy akcje powinny by� w g�rnym wierszu, czy bez znaczenia
    void placeObjects( const QList<QObject*> &editList, QLayout* lowerLayout, QLayout* upperLayout, bool actionsOnTop = false );

private:
    //! flexi widget
    QTabWidget * flexiTabWidget;
    //! sekcja z akcjami zwi�zanymi z edycja dokumentu
    QWidget* editSection;
    //! sekcja z akcjami zwi�zanymi z uk�adem tekstu dokumentu
    QWidget* textSection;
    //! sekcja z akcjami zwi�zanymi z zapisem, odczytem dokumentu
    QWidget* fileSection;

    coreUI::CoreFlexiToolBar * widget;
    //! identyfikator grupy dla miniatur raport�w
    int groupID;
};
typedef utils::shared_ptr<ReportsTabContext> ReportsTabContextPtr;
typedef utils::shared_ptr<const ReportsTabContext> ReportsTabContextConstPtr;


class ReportsTab : public AbstractTab
{
public:
    ReportsTab(const QIcon& icon, const QString& label);
	virtual ~ReportsTab() {}

public:
    virtual QWidget* getMainWidget();
    virtual void registerConxtext( coreUI::IAppUsageContextManager* manager, coreUI::IAppUsageContextPtr parent, QTabWidget * flexiTabWidget ) ;

private:
    QWidget* widget;
    ReportsTabContextPtr context;
};
DEFINE_SMART_POINTERS(ReportsTab);

#endif
