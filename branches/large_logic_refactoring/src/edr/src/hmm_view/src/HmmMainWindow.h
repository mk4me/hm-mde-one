#ifndef HMM_TOOLBOXMAIN_H
#define HMM_TOOLBOXMAIN_H

#include <stack>
#include <boost/bimap.hpp>
#include <boost/bimap/multiset_of.hpp>
#include <core/IVisualizer.h>
#include <QtGui/QMessageBox>
#include <QtGui/QFrame>
#include <QtGui/QToolBox>
#include <timelinelib/IChannel.h>
#include <core/PluginCommon.h>
#include <core/src/EDRTitleBar.h>
#include <core/src/SubjectDataFilters.h>
#include <plugins/c3d/C3DChannels.h>
#include <plugins/subject/ISubjectService.h>
#include <plugins/newTimeline/ITimelineService.h>
#include <plugins/video/Wrappers.h>
#include <plugins/kinematic/Wrappers.h>
#include <core/src/MainWindow.h>
#include "TreeItemHelper.h"
#include <core/src/VisualizerWidget.h>
#include "ui_toolboxmaindeffile.h"
#include <core/src/FlexiTabWidget.h>
#include <core/src/IAppUsageContextManager.h>
#include "TreeBuilder.h"
#include "SummaryWindow.h"
#include "HmmContexts.h"
#include "TreeRefresher.h"

class EDRDockWidgetManager;
class DataFilterWidget;
class AnalisisWidget;
class EDRDockWidgetSet;
class FilterEntryWidget;
class TextEdit;
class ContextEventFilter;
class ContextAction;

//! Klasa realizuje widok aplikacji dla medyków
//! Z czasem klasa zaczela się rozrastac, wymaga glebszej refaktoryzacji
class HmmMainWindow : public core::MainWindow, private Ui::HMMMain, protected IAppUsageContextManager
{
    Q_OBJECT
private:
	friend class ContextEventFilter;
    friend class SummaryWindowController; //tymczasowo

    //! Obserwuje Memory data manager. Jezeli dane się zmienia, to odświeżone zostanie drzewo.
    class DataObserver : public utils::Observer<core::IMemoryDataManager>
    {
    public:
        //! konstruktor 
        //! \param hmm dostarcza drzewo analiz
        DataObserver(HmmMainWindow* hmm) : hmm(hmm), motionsCount(0) {}
        //! wywoływane, gdy dane się zmieniaja, metoda odświeża drzewa analiz
        //! \param subject 
        virtual void update( const core::IMemoryDataManager * subject );

    private:
        //! obiekt, który będzie odświeżany po zmianie danych
        HmmMainWindow* hmm;
        //! do okreslania zmiany stanu
        int motionsCount;
    };
    typedef core::shared_ptr<DataObserver> DataObserverPtr;

    //! służy do przechowywania informacji o utworzonych wizualizatorach i ich seriach, używane w mapie itemHelper -> itemDesc
    struct DataItemDescription 
    {
        //! Konstruktor wypełnia wszystkie pola struktury 
        //! \param visualizer 
        //! \param series 
        //! \param widget 
        DataItemDescription(VisualizerWeakPtr visualizer, const std::vector<core::VisualizerTimeSeriePtr>& series, VisualizerWidget* widget);
        //! serie danych podpięte pod wizualizator, slaby wskaźnik zapobiega "trzymaniu danych"
        std::vector<core::VisualizerTimeSerieWeakPtr> series;
        //! wizualizator z seriami, slaby wskaźnik zapobiega "trzymaniu danych"
        VisualizerWeakPtr visualizer;
        //! widget, w który reprezentuje wizualizator
        VisualizerWidget* visualizerWidget;
    };

public:
    //! 
    HmmMainWindow();
    virtual ~HmmMainWindow();

public:
    //! Natywne dodanie opcji do menu.
    virtual void onAddMenuItem( const std::string& path, bool checkable, bool initialState ) {}
    //! Natywne usunięcie opcji z menu.
    virtual void onRemoveMenuItem( const std::string& path ) {}
    //! Szeroko rozumiana inicjalizacja (managery, pluginy, konteksty, style, widgety...)
    //! \param pluginLoader obiekt przekazywany klasie bazowej (inicjalizacja pluginow i managerów)
    //! \param managersAccessor obiekt przekazywany klasie bazowej (inicjalizacja pluginow i managerów)
    virtual void init( core::PluginLoader* pluginLoader, core::IManagersAccessor * managersAccessor );
    //! \return aktualnie zaladowane sesje pomiarowe
    const std::vector<PluginSubject::SessionConstPtr>& getCurrentSessions();
    //! dodanie głównego elementu (topItem) do drzewa danych w analizach
    //! \param item dodawany element
    void addItemToTree(QTreeWidgetItem* item);
    //! czyszczenie drzewa danych w analizach
    void clearTree();
    //! Pobiera menu kontekstowe dla wybranego elementu drzewa analiz
    //! \param parent rodzic tworzonego menu
    //! \param helper item drzewa, dla którego tworzone jest menu
    //! \return utworzone menu
    QMenu* getContextMenu( QWidget* parent, HmmTreeItem* helper );
    //! \return widget (zakładka) z analizami
    const AnalisisWidget* getAnalisis() const { return analisis; }
    //! Tworzy raport, zakładka z raportami staje się aktywna
    //! \param html tresc raportu w formie ograniczonego przez Qt html-a
    void createRaport( const QString& html );
    //! Ustawienie kontekstu na konkretny wizualizator
    //! \param visWidget wizualizator, dla którego aktywuje się kontekst
    virtual void setCurrentVisualizerActions(VisualizerWidget * visWidget);

public Q_SLOTS:
    //! odświeżenie drzewa danych z zakładki analiz
    void refreshTree();

private Q_SLOTS:
    //! wywoływane po nacisnieciu about
    void onAbout();
    //! Po zniszczeniu wizualizatora trzeba go wyrejstrować z kontekstów
    //! \param visualizer niszczony wizualizator
    void visualizerDestroyed(QObject * visualizer);
    //! kliknięto filtr, trzeba utworzyć drzewo
    //! \param entry filtr, który został kliknięty
    void filterClicked(FilterEntryWidget* entry);
    //! Kliknięto prawym przyciskiem na drzewie, trzeba pokazac menu kontekstowe
    //! \param pos pozycja, w która kliknięto
    void onTreeContextMenu(const QPoint & pos);
    //! Dodanie pixmapy do raportów
    //! \param pixmap dodawana pixmapa
    void addToRaports(const QPixmap& pixmap);
    //! tworzy nowy wizualizator na podstwie wywołanej akcji
    void createNewVisualizer();
    //! Na podstawie wybranego elementu drzewa analiz tworzy i dodaje wizualizator w nowej zakładce
    void createVisualizerInNewSet();
    //! Na podstawie wybranego elementu drzewa analiz tworzy i dodaje wizualizator w ustalonym miejscu
    void removeFromVisualizer();
    //! Tworzy wizualizator z wszystkimi seriami danego typu wystepującymi w sesji
    void allFromSession();
    //! Tworzy wizualizator 2D z wszystkimi seriami X danego typu wystepującymi w sesji
    void allXFromSession();
    //! Tworzy wizualizator 2D z wszystkimi seriami Y danego typu wystepującymi w sesji
    void allYFromSession();
    //! Tworzy wizualizator 2D z wszystkimi seriami Z danego typu wystepującymi w sesji
    void allZFromSession();
    //! Tworzy wizualizator 2D z wszystkimi seriami(o konkretnej nazwie) wystepującymi w sesji, które zostały znormalizowane w kontekscie kroków lewej nogi
    void allLeftNormalized();
    //! Tworzy wizualizator 2D z wszystkimi seriami(o konkretnej nazwie) wystepującymi w sesji, które zostały znormalizowane w kontekscie kroków prawej nogi
    void allRightNormalized();
    //! Tworzy wizualizator 2D na podstawie danych (przypisanych do elementu drzewa analiz), które zostały znormalizowane w kontekscie kroków lewej nogi
    void normalizedLeftChart();
    //! Tworzy wizualizator 2D na podstawie danych (przypisanych do elementu drzewa analiz), które zostały znormalizowane w kontekscie kroków prawej nogi
    void normalizedRightChart();
    //! Na podstawie elementu drzewa usuwa związane z nim serie danych z wszystkich wizualizatorów
    void removeFromAll();
    //! Na podstawie elementu drzewa dodaje związane z nim serie do podswietlonego wizualizatora
    void addToVisualizer();
    //! Podswietla wizualizator na podstawie obiektu ContextAction
    void menuHighlightVisualizer(QAction* action = nullptr);
    //! zmieniła się aktywność jednej z grupy filtrów, może być aktywna tylko jedna
    //! \param active czy grupa jest aktywna, jeśli tak to wszystkie pozostale staja się nieaktywne
    void filterGroupActivated(bool active);
    //! został kliknięty przycisk z głównym kontekstem aplikacji (Dane, analizy, ...)
    //! \param checked 
    void onToolButton(bool checked);

private:
    //! Timeline staje się widoczny, o ile już nie jest
    void showTimeline();
    //! Czy pod element drzewa podpięte są jakieś dane
    //! \param item badany element
    //! \return 
    bool isDataItem(QTreeWidgetItem * item);
    //! Tworzy zakładke z filtrami
    void createFilterTabs();
    //! Tworzy zakładke z filtrami (główne filtry)
    void createFilterTab1();
    //! Tworzy zakładke z filtrami (filtry dla jednostek chorobowych)
    void createFilterTab2();
    //! metoda przechodzi przez całą kolekcje i usuwa te wpisy, których weak pointery przestały już istnieć
    //! \param multimap sprawdzana kolekcja
    void dropUnusedElements(std::multimap<TreeItemHelperPtr, DataItemDescription>& multimap);
    //! Na podstawie wybranego elementu drzewa analiz tworzy i dodaje wizualizator w ustalonym miejscu
    //! \param hmmItem wybrany item, na podstwie którego tworzony jest wizualizator
    //! \param dockSet set, do którego ma być dodany element, jeśli jest nullptr to wizualizator dodawany jest tam, gdzie jest miejsce
    //! \return utworzony dockWidget z wizualizatorem
    VisualizerWidget* createAndAddDockVisualizer( HmmTreeItem* hmmItem, EDRDockWidgetSet* dockSet);
    //! wywoływane, gdy jakis widget dostaje focusa.
    //! wykorzystywane na potrzeby kontekstów flexi bara
    //! Hack - dodatkowo podpina się pod to okienko summary
    //! \param widget widget, który dostaje focusa
    void activateContext(QWidget * widget);
    //! wywoływane, gdy obsługiwany widget nie ma już focusa
    //! \param widget widget, który stracil focus
    void deactivateContext(QWidget * widget);
    //! Opakowuje wizualizator w DockWidget
    //! \param visualizer wizualizator do opakowania
    VisualizerWidget* createDockVisualizer(const VisualizerPtr & visualizer);
    //! Rejestruje widget w kontekstach
    //! \param titleBar 
    //! \param visualizerDockWidget 
    //! \param visualizer 
    void registerVisualizerContext( EDRTitleBar * titleBar, VisualizerWidget* visualizerDockWidget, const VisualizerPtr & visualizer );	
    //! Na podstawie wybranego elementu drzewa analiz tworzy i dodaje wizualizator w ustalonym miejscu
    //! \param item wybrany item, na podstwie którego tworzony jest wizualizator
    //! \param dockSet set, do którego ma być dodany element, jeśli jest nullptr to wizualizator dodawany jest tam, gdzie jest miejsce
    void createNewVisualizer( HmmTreeItem* item, EDRDockWidgetSet* dockSet = nullptr );
    //! Tworzy wizualizator 2D z wszystkimi seriami(o konkretnej nazwie) wystepującymi w sesji, które zostały znormalizowane w kontekscie kroków
    //! \param helper element, dla którego zostanie utworzony wizualizator
    //! \param context kontekst kroków (lewy, prawy)
    void createNormalizedFromAll( NewVector3ItemHelperPtr helper, c3dlib::C3DParser::IEvent::Context context );
    //! Tworzy wizualizator 2D z wszystkimi seriami(o konkretnej nazwie) wystepującymi w sesji, które zostały znormalizowane w kontekscie kroków
    //! \param helper element, dla którego zostanie utworzony wizualizator
    //! \param context kontekst kroków (lewy, prawy)
    void createNormalized( NewVector3ItemHelperPtr helper, c3dlib::C3DParser::IEvent::Context context );
    //! Tworzy wizualizator 2D z wszystkimi seriami(o konkretnej nazwie i konkretnym kanale) wystepującymi w sesji
    //! \param helper element, dla którego zostanie utworzony wizualizator
    //! \param channelNo numer kanału (0 - X, 1 - Y, 2 - Z)
    void allTFromSession( NewVector3ItemHelperPtr helper, int channelNo );
    //! Usuwa serie z wizualizatora
    //! \param action akcja zawierająca dane o seriach i wizualizatorze
    //! \param once czy usunąć z wszystkich, czy tylko ze wskazanego wizualizatora
    void removeFromVisualizers( ContextAction* action, bool once );
    //! podswietla wizualizator zolta ramka, inne traca podswietlenie
    //! \param visualizer wizualizator do podswietlenia
    void highlightVisualizer( const VisualizerPtr& visualizer  );
    
	typedef boost::bimap<QWidget*, boost::bimaps::multiset_of<QWidget*>> DerrivedContextWidgets;
	typedef std::pair<AppUsageContextPtr, QWidget*> ContextState;

private:
    //! kolekcja z aktualnie obsługiwanymi sesjami
    std::vector<PluginSubject::SessionConstPtr> currentSessions;
    // TODO, tu jest blad, obiekt jest zawsze nullem
    VisualizerWidget* currentVisualizer;    
    //! górny widget aplikacji gdzie trafiaja dock Widgety
    EDRDockWidgetManager* topMainWindow;
    //! dolny widget aplikacji na timeline
    QMainWindow* bottomMainWindow;
    //! widget, gdzie trafiaja filtry analiz
    std::vector<DataFilterWidget*> dataFilterWidgets;
    //! struktura z informacjami o stworzonych wizualizatorach, ich seriach oraz z którego elementu powstały
    std::multimap<TreeItemHelperPtr, DataItemDescription> items2Descriptions;
    //! widget z analizami
    AnalisisWidget* analisis;
    //! zakładka z danymi
    QWidget* data;
    //! zakładka z operacjami (konsola)
    QWidget* operations;
    //! zakładka z raportami
    TextEdit* raports;
    //! 
    FlexiTabWidget * flexiTabWidget;
    //!
    //FlexiTabWidget::GUIID visualizerGroupID;
    //! mapa [przycisk -> zakładka]
    std::map<QWidget*, QWidget*> button2TabWindow;
    //!
	std::map<QWidget*, ContextState> contextStates;
    //! aktywny przycisk oznaczający aktywna zakładke (dane, analizy, operacje... )
    QToolButton* currentButton;
    //! obiekt informuje o zmianie danych w data managerze
    DataObserverPtr dataObserver;
    //! kontekst wizualizatorów
    AppUsageContextPtr visualizerUsageContext;
    //! kontekst drzewa danych w analizach
    HMMTreeItemUsageContextPtr treeUsageContext;
    //! kontekst dla miniaturek raportów
    RaportsThumbnailsContextPtr raportsThumbnailsContext;
    //! kontekst dla kontrolki TextEdit z raportami
    RaportsTabContextPtr raportsTabContext;
    //! kontekst dla zakładki z danymi
	AppUsageContextPtr dataContext;
    //! kontekst dla zakładki z analizami
	AppUsageContextPtr analisisContext;
    //! kontekst dla zakładki z raportami
	AppUsageContextPtr reportsContext;
    //! grupuje konteksty zakładek
	std::set<QWidget*> plainContextWidgets;
    //! 
	DerrivedContextWidgets derrivedContextWidgets;
    //! filtruje zmiany focusów widgetów, obsługuje zdarzenia dla kontekstów
	ContextEventFilter* contextEventFilter;
    //! okienko summary (podsumowanie, informacje o wybranch danych)
    SummaryWindowPtr summaryWindow;
    //! kontrolek okienka z podsumowaniami
    SummaryWindowController* summaryWindowController;
    //! zarządza odświeżaniem drzewa danych w analizach
    TreeRefresher treeRefresher;
};


#endif // TOOLBOXMAIN_H
