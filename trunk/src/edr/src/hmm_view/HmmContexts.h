/********************************************************************
	created:	2012/01/18
	created:	18:1:2012   11:35
	filename: 	HmmContexts.h
	author:		Wojciech Kniec

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_HMM__HMMCONTEXTS_H__
#define HEADER_GUARD_HMM__HMMCONTEXTS_H__

#include <boost/tuple/tuple.hpp>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QComboBox>
#include <QtGui/QTreeWidget>
#include "FlexiTabWidget.h"
#include "IAppUsageContextManager.h"

//! Kontekst bazowy dla kontekst�w aplikacji medycznej
class HMMEmptyContext : public IAppUsageContext
{
public:
	HMMEmptyContext() {}
	virtual ~HMMEmptyContext() {}

public:
	//! wywo�ywane, po aktywowaniu si� kontekstu
	//! \param contextWidget wiget, kt�ry wywo�a� kontekst
	virtual void activateContext(QWidget * contextWidget) {}
	//! wywo�ywane, gdy kontekst przestaje by� aktywny
	//! \param nextContextWidget 
	//! \param refresh 
	virtual void deactivateContext(QWidget * nextContextWidget, bool refresh) {}
};

//! Placeholder dla kontekstu danych
class HMMDataContext : public HMMEmptyContext
{

};

class TreeRefresher;
//! kontekst wywo�ywany dla zak�adki analiz
class HMMAnalysisContext : public IAppUsageContext
{
public:
	//! konstruktor
	//! \param refresher klasa s�u�y do od�wie�ania drzewa danych
	HMMAnalysisContext(TreeRefresher * refresher);
	//! kontekst staje si� aktywny, je�li jest taka potrzeba, to od�wie�ane jest drzewo danych
	//! \param contextWidget 
	virtual void activateContext(QWidget * contextWidget);
	//! kontekst jest deaktywowany, nie ma potrzeby od�wie�a� drzewa analiz -> zostaje to zablokowane
	//! \param nextContextWidget 
	//! \param refresh 
	virtual void deactivateContext(QWidget * nextContextWidget, bool refresh);

private:
    //! klasa s�u�y do od�wie�ania drzewa danych
	TreeRefresher * refresher;
};

//! placeholder dla kontekstu raport�w
class HMMReportContext : public HMMEmptyContext
{

};

//! kontekst dla wizualizator�w
class HMMVisualizerUsageContext : public IAppUsageContext
{
public:
    HMMVisualizerUsageContext(FlexiTabWidget * flexiTabWidget);
public:
    //! wywo�ywane, po aktywowaniu si� kontekstu
    //! \param contextWidget wiget, kt�ry wywo�a� kontekst
    virtual void activateContext(QWidget * contextWidget);
    //! wywo�ywane, gdy kontekst przestaje by� aktywny
    //! \param nextContextWidget 
    //! \param refresh
    virtual void deactivateContext(QWidget * nextContextWidget, bool refresh);
    //! wizualizator jest rejestrowany, tworzony jest flexi tabdla niego
    virtual void onRegisterContextWidget(QWidget * contextWidget);
    //! wizualizator jest zamkni�ty, usuwany jest flexitab dla niego
    virtual void onUnregisterContextWidget(QWidget * contextWidget);

private:
    //! flexi tab z akcjami
    FlexiTabWidget * flexiTabWidget;
    //! identyfikator grupy dla wizualizator�w
    FlexiTabWidget::GUIID visualizerGroupID;
    //! zbi�r sekcji
    std::set<FlexiTabWidget::GUIID> visualizerSectionsIDs;
    //! mapa [widget kontekstu - > [nazwa grupy -> widget flexi]]
    std::map<QWidget*, std::map<QString, QWidget *>> visualizersData;

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
    HMMTreeItemUsageContext(FlexiTabWidget * flexiTabWidget, HmmMainWindow* hmm);
public:
    //! wywo�ywane, po aktywowaniu si� kontekstu, do flexi bara trafi� akcje zwi�zane z drzewem analiz
    //! \param contextWidget wiget, kt�ry wywo�a� kontekst
    virtual void activateContext(QWidget * contextWidget);
    //! wywo�ywane, gdy kontekst przestaje by� aktywny
    //! z flexibara znikaj� akcje zwi�zane z drzewem analiz
    virtual void deactivateContext(QWidget * nextContextWidget, bool refresh);
    //! rejestracja drzewa analiz
    virtual void onRegisterContextWidget(QWidget * contextWidget);
    //! wyrejstrowanie drzewa analiz
    virtual void onUnregisterContextWidget(QWidget * contextWidget);

public slots:
    //! na podstawie aktualnego stanu drzewa od�wie�a flexiwidget
    void refresh();

private slots:
    //! zmieni� si� zaznaczony element drzewa analiz, nale�y zmieni� flexi widget
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
    FlexiTabWidget * flexiTabWidget;
    //! sekcja obs�ugiwanego flexi widgeta
    QWidget* flexiSection;
    //! identyfikator grupy dla drzewa
    FlexiTabWidget::GUIID groupID;
    //! g��wne okno aplikacji
    HmmMainWindow* hmm;
};
typedef core::shared_ptr<HMMTreeItemUsageContext> HMMTreeItemUsageContextPtr;
typedef core::shared_ptr<const HMMTreeItemUsageContext> HMMTreeItemUsageContextConstPtr;

//! kontekst dla zak�adki z miniaturkami obraz�w do raportu
class RaportsThumbnailsContext : public QObject, public IAppUsageContext
{
    Q_OBJECT;
public:
    //! konstruktor
    //! \param flexiTabWidget flexi widget kontekstu
    //! \param hmm g��wne okno aplikacji
    RaportsThumbnailsContext(FlexiTabWidget * flexiTabWidget, HmmMainWindow* hmm);
public:
    //! wywo�ywane, po aktywowaniu si� kontekstu, do flexi bara trafi� akcje zwi�zane z tworzeniem raport�w
    //! \param contextWidget wiget, kt�ry wywo�a� kontekst
    virtual void activateContext(QWidget * contextWidget);
    //! wywo�ywane, gdy kontekst przestaje by� aktywny
    //! z flexibara znikaj� akcje zwi�zane z tworzeniem raport�w
    virtual void deactivateContext(QWidget * nextContextWidget, bool refresh);
    //! rejestracja widgeta z miniaturkami raport�w
    virtual void onRegisterContextWidget(QWidget * contextWidget);
    //! wyrejstrowanie widgeta z miniaturkami raport�w
    virtual void onUnregisterContextWidget(QWidget * contextWidget);

private slots:
    //! Utworzenie raportu na podstawie szablonu i stylu, z do��czonymi obrazami
    void createRaport();

private:
    //! flexi widget
    FlexiTabWidget * flexiTabWidget;
    //! sekcja obs�ugiwanego flexi widgeta
    QWidget* flexiSection;
    //! identyfikator grupy dla miniatur raport�w
    FlexiTabWidget::GUIID groupID;
    //! g��wne okno aplikacji
    HmmMainWindow* hmm;
    //! nazwa tworzonego raportu
    QPlainTextEdit* projectName;
    //! kombo box, z szablonami raport�w
    QComboBox* projectTemplate;
    //! kombo box ze stylami raport�w
    QComboBox* cssCombo;
};
typedef core::shared_ptr<RaportsThumbnailsContext> RaportsThumbnailsContextPtr;
typedef core::shared_ptr<const RaportsThumbnailsContext> RaportsThumbnailsContextConstPtr;

//! kontekst dla zak�adki z raportami
class RaportsTabContext : public QObject, public IAppUsageContext
{
    Q_OBJECT;
public:
    //! konstruktor
    //! \param flexiTabWidget flexi widget kontekstu
    //! \param hmm g��wne okno aplikacji
    RaportsTabContext(FlexiTabWidget * flexiTabWidget, HmmMainWindow* hmm);

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
    FlexiTabWidget * flexiTabWidget;
    //! sekcja z akcjami zwi�zanymi z edycja dokumentu
    QWidget* editSection;
    //! sekcja z akcjami zwi�zanymi z uk�adem tekstu dokumentu
    QWidget* textSection;
    //! sekcja z akcjami zwi�zanymi z zapisem, odczytem dokumentu
    QWidget* fileSection;
    //! identyfikator grupy dla miniatur raport�w
    FlexiTabWidget::GUIID groupID;
    //! g��wne okno aplikacji
    HmmMainWindow* hmm;
};
typedef core::shared_ptr<RaportsTabContext> RaportsTabContextPtr;
typedef core::shared_ptr<const RaportsTabContext> RaportsTabContextConstPtr;

#endif
