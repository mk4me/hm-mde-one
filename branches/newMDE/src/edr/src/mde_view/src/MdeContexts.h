/********************************************************************
	created:	2012/01/18
	created:	18:1:2012   11:35
	filename: 	HmmContexts.h
	author:		Wojciech Kniec

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_MDE_MDECONTEXTS_H__
#define HEADER_GUARD_MDE_MDECONTEXTS_H__

#include <QtGui/QPlainTextEdit>
#include <QtGui/QComboBox>
#include <QtGui/QTreeWidget>
#include <boost/tuple/tuple.hpp>
#include "IAppUsageContext.h"
#include <coreui/CoreFlexiToolBar.h>

//! Kontekst bazowy dla kontekstów aplikacji medycznej
class MdeEmptyContext : public IAppUsageContext
{
public:
	MdeEmptyContext() {}
	virtual ~MdeEmptyContext() {}

public:
	//! wywoływane, po aktywowaniu się kontekstu
	//! \param contextWidget wiget, który wywołał kontekst
	virtual void activateContext(QWidget * contextWidget) {}
	//! wywoływane, gdy kontekst przestaje być aktywny
	//! \param nextContextWidget 
	//! \param refresh 
	virtual void deactivateContext(QWidget * nextContextWidget, bool refresh) {}
};

//! Placeholder dla kontekstu danych
class MdeDataContext : public MdeEmptyContext
{

};

//class TreeRefresher;
////! kontekst wywoływany dla zakładki analiz
//class MdeAnalysisContext : public IAppUsageContext
//{
//public:
//	//! konstruktor
//	//! \param refresher klasa służy do odświeżania drzewa danych
//	MdeAnalysisContext(TreeRefresher * refresher);
//	//! kontekst staje się aktywny, jeśli jest taka potrzeba, to odświeżane jest drzewo danych
//	//! \param contextWidget 
//	virtual void activateContext(QWidget * contextWidget);
//	//! kontekst jest deaktywowany, nie ma potrzeby odświeżać drzewa analiz -> zostaje to zablokowane
//	//! \param nextContextWidget 
//	//! \param refresh 
//	virtual void deactivateContext(QWidget * nextContextWidget, bool refresh);
//
//private:
//    //! klasa służy do odświeżania drzewa danych
//	TreeRefresher * refresher;
//};

//! placeholder dla kontekstu raportów
class MdeReportContext : public MdeEmptyContext
{

};

////! placeholder dla kontekstu vdf
//class MdeVdfContext : public MdeEmptyContext
//{
//
//};
//
////! kontekst dla wizualizatorów
//class MdeVVdfUsageContext : public IAppUsageContext
//{
//public:
//    MdeVVdfUsageContext(QTabWidget * flexiTabWidget);
//public:
//    //! wywoływane, po aktywowaniu się kontekstu
//    //! \param contextWidget wiget, który wywołał kontekst
//    virtual void activateContext(QWidget * contextWidget);
//    //! wywoływane, gdy kontekst przestaje być aktywny
//    //! \param nextContextWidget 
//    //! \param refresh
//    virtual void deactivateContext(QWidget * nextContextWidget, bool refresh);
//    //! widget jest rejestrowany, tworzony jest flexi tabdla niego
//    virtual void onRegisterContextWidget(QWidget * contextWidget);
//    //! widget jest zamknięty, usuwany jest flexitab dla niego
//    virtual void onUnregisterContextWidget(QWidget * contextWidget);
//
//private:
//    //! flexi tab z akcjami
//    QTabWidget * flexiTabWidget;
//    //! zbiór sekcji
//
//    std::list<coreUI::CoreFlexiToolBarSection*> sections;
//
//    coreUI::CoreFlexiToolBar * widget;
//    //! identyfikator grupy dla miniatur raportów
//    int groupID;
//
//};

////! kontekst dla wizualizatorów
//class MdeVisualizerUsageContext : public IAppUsageContext
//{
//public:
//    MdeVisualizerUsageContext(QTabWidget * flexiTabWidget);
//public:
//    //! wywoływane, po aktywowaniu się kontekstu
//    //! \param contextWidget wiget, który wywołał kontekst
//    virtual void activateContext(QWidget * contextWidget);
//    //! wywoływane, gdy kontekst przestaje być aktywny
//    //! \param nextContextWidget 
//    //! \param refresh
//    virtual void deactivateContext(QWidget * nextContextWidget, bool refresh);
//    //! wizualizator jest rejestrowany, tworzony jest flexi tabdla niego
//    virtual void onRegisterContextWidget(QWidget * contextWidget);
//    //! wizualizator jest zamknięty, usuwany jest flexitab dla niego
//    virtual void onUnregisterContextWidget(QWidget * contextWidget);
//
//private:
//
//	struct UsageContextData
//	{
//		QString name;
//		QWidget * widget;
//		QIcon icon;
//	};
//
//private:
//    //! flexi tab z akcjami
//    QTabWidget * flexiTabWidget;
//    //! identyfikator grupy dla wizualizatorów
//    int visualizerGroupID;
//    //! zbiór sekcji
//    std::list<coreUI::CoreFlexiToolBarSection*> visualizerSections;
//    //! mapa [widget kontekstu - > [nazwa grupy -> widget flexi]]
//    std::map<QWidget*, UsageContextData> visualizersData;
//
//};


#endif
