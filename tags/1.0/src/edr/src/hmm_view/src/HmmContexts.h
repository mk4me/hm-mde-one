/********************************************************************
	created:	2012/01/18
	created:	18:1:2012   11:35
	filename: 	HmmContexts.h
	author:		Wojciech Kniec

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_HMM__HMMCONTEXTS_H__
#define HEADER_GUARD_HMM__HMMCONTEXTS_H__

#include <QtGui/QPlainTextEdit>
#include <QtGui/QComboBox>
#include <QtGui/QTreeWidget>
#include <boost/tuple/tuple.hpp>
#include "IAppUsageContext.h"
#include <coreui/CoreFlexiToolBar.h>

//! Kontekst bazowy dla kontekstów aplikacji medycznej
class HMMEmptyContext : public IAppUsageContext
{
public:
	HMMEmptyContext() {}
	virtual ~HMMEmptyContext() {}

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
class HMMDataContext : public HMMEmptyContext
{

};

class TreeRefresher;
//! kontekst wywoływany dla zakładki analiz
class HMMAnalysisContext : public IAppUsageContext
{
public:
	//! konstruktor
	//! \param refresher klasa służy do odświeżania drzewa danych
	HMMAnalysisContext(TreeRefresher * refresher);
	//! kontekst staje się aktywny, jeśli jest taka potrzeba, to odświeżane jest drzewo danych
	//! \param contextWidget 
	virtual void activateContext(QWidget * contextWidget);
	//! kontekst jest deaktywowany, nie ma potrzeby odświeżać drzewa analiz -> zostaje to zablokowane
	//! \param nextContextWidget 
	//! \param refresh 
	virtual void deactivateContext(QWidget * nextContextWidget, bool refresh);

private:
    //! klasa służy do odświeżania drzewa danych
	TreeRefresher * refresher;
};

//! placeholder dla kontekstu raportów
class HMMReportContext : public HMMEmptyContext
{

};

//! placeholder dla kontekstu vdf
class HMMVdfContext : public HMMEmptyContext
{

};

//! kontekst dla wizualizatorów
class HMMVVdfUsageContext : public IAppUsageContext
{
public:
    HMMVVdfUsageContext(QTabWidget * flexiTabWidget);
public:
    //! wywoływane, po aktywowaniu się kontekstu
    //! \param contextWidget wiget, który wywołał kontekst
    virtual void activateContext(QWidget * contextWidget);
    //! wywoływane, gdy kontekst przestaje być aktywny
    //! \param nextContextWidget 
    //! \param refresh
    virtual void deactivateContext(QWidget * nextContextWidget, bool refresh);
    //! widget jest rejestrowany, tworzony jest flexi tabdla niego
    virtual void onRegisterContextWidget(QWidget * contextWidget);
    //! widget jest zamknięty, usuwany jest flexitab dla niego
    virtual void onUnregisterContextWidget(QWidget * contextWidget);

private:
    //! flexi tab z akcjami
    QTabWidget * flexiTabWidget;
    //! zbiór sekcji

    std::list<coreUI::CoreFlexiToolBarSection*> sections;

    coreUI::CoreFlexiToolBar * widget;
    //! identyfikator grupy dla miniatur raportów
    int groupID;

};

//! kontekst dla wizualizatorów
class HMMVisualizerUsageContext : public IAppUsageContext
{
public:
    HMMVisualizerUsageContext(QTabWidget * flexiTabWidget);
public:
    //! wywoływane, po aktywowaniu się kontekstu
    //! \param contextWidget wiget, który wywołał kontekst
    virtual void activateContext(QWidget * contextWidget);
    //! wywoływane, gdy kontekst przestaje być aktywny
    //! \param nextContextWidget 
    //! \param refresh
    virtual void deactivateContext(QWidget * nextContextWidget, bool refresh);
    //! wizualizator jest rejestrowany, tworzony jest flexi tabdla niego
    virtual void onRegisterContextWidget(QWidget * contextWidget);
    //! wizualizator jest zamknięty, usuwany jest flexitab dla niego
    virtual void onUnregisterContextWidget(QWidget * contextWidget);

private:

	struct UsageContextData
	{
		QString name;
		QWidget * widget;
		QIcon icon;
	};

private:
    //! flexi tab z akcjami
    QTabWidget * flexiTabWidget;
    //! identyfikator grupy dla wizualizatorów
    int visualizerGroupID;
    //! zbiór sekcji
    std::list<coreUI::CoreFlexiToolBarSection*> visualizerSections;
    //! mapa [widget kontekstu - > [nazwa grupy -> widget flexi]]
    std::map<QWidget*, UsageContextData> visualizersData;

};


class HmmMainWindow;
class HmmTreeItem;
//! kontekst dla drzewa analiz
class HMMTreeItemUsageContext : public QObject, public IAppUsageContext
{
    Q_OBJECT;
public:
    //! konstruktor
    //! \param flexiTabWidget 
    //! \param hmm 
    HMMTreeItemUsageContext(QTabWidget * flexiTabWidget, HmmMainWindow* hmm);
public:
    //! wywoływane, po aktywowaniu się kontekstu, do flexi bara trafią akcje związane z drzewem analiz
    //! \param contextWidget wiget, który wywołał kontekst
    virtual void activateContext(QWidget * contextWidget);
    //! wywoływane, gdy kontekst przestaje być aktywny
    //! z flexibara znikają akcje związane z drzewem analiz
    virtual void deactivateContext(QWidget * nextContextWidget, bool refresh);
    //! rejestracja drzewa analiz
    virtual void onRegisterContextWidget(QWidget * contextWidget);
    //! wyrejstrowanie drzewa analiz
    virtual void onUnregisterContextWidget(QWidget * contextWidget);

public Q_SLOTS:
    //! na podstawie aktualnego stanu drzewa odświeża flexiwidget
    void refresh();

private Q_SLOTS:
    //! zmienił się zaznaczony element drzewa analiz, należy zmienić flexi widget
    //! \param current aktualny element drzewa
    //! \param previous poprzedni element drzewa
    void itemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);

private:
    //! odtworzenie flexi widgeta na podstawie wskazywanego elementu drzewa
    //! \param flexiSection modyfikowana sekcja
    //! \param helper aktualnie aktywny alement drzewa analiz
    void recreateFlexiSectionWidget(QWidget* flexiSection, HmmTreeItem* helper);

private:
    //! flexi widget
    QTabWidget * flexiTabWidget;
    //! sekcja obsługiwanego flexi widgeta
    QWidget* flexiContent;

	QWidget * contextWidgetActions;

	coreUI::CoreFlexiToolBarSection * flexiSection;
    //! identyfikator grupy dla drzewa
    int groupID;
    //! główne okno aplikacji
    HmmMainWindow* hmm;
};
typedef utils::shared_ptr<HMMTreeItemUsageContext> HMMTreeItemUsageContextPtr;
typedef utils::shared_ptr<const HMMTreeItemUsageContext> HMMTreeItemUsageContextConstPtr;

//! kontekst dla zakładki z miniaturkami obrazów do raportu
class RaportsThumbnailsContext : public QObject, public IAppUsageContext
{
    Q_OBJECT;
public:
    //! konstruktor
    //! \param flexiTabWidget flexi widget kontekstu
    //! \param hmm główne okno aplikacji
    RaportsThumbnailsContext(QTabWidget * flexiTabWidget, HmmMainWindow* hmm);
public:
    //! wywoływane, po aktywowaniu się kontekstu, do flexi bara trafią akcje związane z tworzeniem raportów
    //! \param contextWidget wiget, który wywołał kontekst
    virtual void activateContext(QWidget * contextWidget);
    //! wywoływane, gdy kontekst przestaje być aktywny
    //! z flexibara znikają akcje związane z tworzeniem raportów
    virtual void deactivateContext(QWidget * nextContextWidget, bool refresh);
    //! rejestracja widgeta z miniaturkami raportów
    virtual void onRegisterContextWidget(QWidget * contextWidget);
    //! wyrejstrowanie widgeta z miniaturkami raportów
    virtual void onUnregisterContextWidget(QWidget * contextWidget);

private Q_SLOTS:
    //! Utworzenie raportu na podstawie szablonu i stylu, z dołączonymi obrazami
    void createRaport();

private:
    //! flexi widget
    QTabWidget * flexiTabWidget;
    //! sekcja obsługiwanego flexi widgeta
    QWidget* flexiSection;
    //! identyfikator grupy dla miniatur raportów
    int groupID;
    //! główne okno aplikacji
    HmmMainWindow* hmm;
    //! nazwa tworzonego raportu
    QPlainTextEdit* projectName;
    //! kombo box, z szablonami raportów
    QComboBox* projectTemplate;
    //! kombo box ze stylami raportów
    QComboBox* cssCombo;
};
typedef utils::shared_ptr<RaportsThumbnailsContext> RaportsThumbnailsContextPtr;
typedef utils::shared_ptr<const RaportsThumbnailsContext> RaportsThumbnailsContextConstPtr;

//! kontekst dla zakładki z raportami
class RaportsTabContext : public QObject, public IAppUsageContext
{
    Q_OBJECT;
public:
    //! konstruktor
    //! \param flexiTabWidget flexi widget kontekstu
    //! \param hmm główne okno aplikacji
    RaportsTabContext(QTabWidget * flexiTabWidget, HmmMainWindow* hmm);

public:
    //! wywoływane, po aktywowaniu się kontekstu, do flexi bara trafią akcje związane z oknem raportów
    //! \param contextWidget wiget, który wywołał kontekst
    virtual void activateContext(QWidget * contextWidget);
    //! wywoływane, gdy kontekst przestaje być aktywny
    //! z flexibara znikają akcje związane z oknem raportów
    virtual void deactivateContext(QWidget * nextContextWidget, bool refresh);
    //! rejestracja okna raportów
    virtual void onRegisterContextWidget(QWidget * contextWidget);
    //! wyrejstrowanie okna raportów
    virtual void onUnregisterContextWidget(QWidget * contextWidget);

private:
    //! tworzy zbiorczy widget, który jest kontenerem dla dwóch wierszy widgetów potomnych
    //! \return krotka z utworzonym widgetem oraz layoutami dla wiersza górnego i dolnego
    boost::tuple<QWidget*, QLayout*, QLayout*> createTwoLineWidget();
    //! automatycznie rozmieszcza widgety w dwóch wierszach
    //! \param editList lista z akcjami i obiektami
    //! \param lowerLayout dolny wiersz
    //! \param upperLayout górny wiersz
    //! \param actionsOnTop czy akcje powinny być w górnym wierszu, czy bez znaczenia
    void placeObjects( const QList<QObject*> &editList, QLayout* lowerLayout, QLayout* upperLayout, bool actionsOnTop = false );

private:
    //! flexi widget
    QTabWidget * flexiTabWidget;
    //! sekcja z akcjami związanymi z edycja dokumentu
    QWidget* editSection;
    //! sekcja z akcjami związanymi z układem tekstu dokumentu
    QWidget* textSection;
    //! sekcja z akcjami związanymi z zapisem, odczytem dokumentu
    QWidget* fileSection;

	coreUI::CoreFlexiToolBar * widget;
    //! identyfikator grupy dla miniatur raportów
    int groupID;
    //! główne okno aplikacji
    HmmMainWindow* hmm;
};
typedef utils::shared_ptr<RaportsTabContext> RaportsTabContextPtr;
typedef utils::shared_ptr<const RaportsTabContext> RaportsTabContextConstPtr;

#endif
