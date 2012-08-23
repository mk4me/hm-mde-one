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
#include <core/EDRTitleBar.h>
#include <core/SubjectDataFilters.h>
#include <plugins/c3d/C3DChannels.h>
#include <plugins/subject/ISubjectService.h>
#include <plugins/newTimeline/ITimelineService.h>
#include <plugins/video/Wrappers.h>
#include <plugins/kinematic/Wrappers.h>
#include "MainWindow.h"
#include "TreeItemHelper.h"
#include "VisualizerWidget.h"
#include "ui_toolboxmaindeffile.h"
#include "FlexiTabWidget.h"
#include "IAppUsageContextManager.h"
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

//! Klasa realizuje widok aplikacji dla medykow
//! Z czasem klasa zaczela siê rozrastac, wymaga glebszej refaktoryzacji
class HmmMainWindow : public core::MainWindow, private Ui::HMMMain, protected IAppUsageContextManager
{
    Q_OBJECT
private:
	friend class ContextEventFilter;
    friend class SummaryWindowController; //tymczasowo

    //! Obserwuje Memory data manager. Jezeli dane siê zmienia, to odswiezone zostanie drzewo.
    class DataObserver : public utils::Observer<core::IMemoryDataManager>
    {
    public:
        //! 
        //! \param hmm 
        DataObserver(HmmMainWindow* hmm) : hmm(hmm), motionsCount(0) {}
        //! wywo³ywane, gdy dane siê zmieniaja, metoda odœwie¿a drzewa analiz
        //! \param subject 
        virtual void update( const core::IMemoryDataManager * subject );

    private:
        //! obiekt, który bêdzie odœwie¿any po zmianie danych
        HmmMainWindow* hmm;
        //! do okreslania zmiany stanu
        int motionsCount;
    };
    typedef core::shared_ptr<DataObserver> DataObserverPtr;

    //! s³u¿y do przechowywania informacji o utworzonych wizualizatorach i ich seriach, u¿ywane w mapie itemHelper -> itemDesc
    struct DataItemDescription 
    {
        //! Konstruktor wype³nia wszystkie pola struktury 
        //! \param visualizer 
        //! \param series 
        //! \param widget 
        DataItemDescription(VisualizerWeakPtr visualizer, const std::vector<core::VisualizerTimeSeriePtr>& series, VisualizerWidget* widget);
        //! serie danych podpiête pod wizualizator, slaby wskaŸnik zapobiega "trzymaniu danych"
        std::vector<core::VisualizerTimeSerieWeakPtr> series;
        //! wizualizator z seriami, slaby wskaŸnik zapobiega "trzymaniu danych"
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
    //! Natywne usuniêcie opcji z menu.
    virtual void onRemoveMenuItem( const std::string& path ) {}
    //! Szeroko rozumiana inicjalizacja (managery, pluginy, konteksty, style, widgety...)
    //! \param pluginLoader obiekt przekazywany klasie bazowej (inicjalizacja pluginow i managerow)
    //! \param managersAccessor obiekt przekazywany klasie bazowej (inicjalizacja pluginow i managerow)
    virtual void init( core::PluginLoader* pluginLoader, core::IManagersAccessor * managersAccessor );
    //! \return aktualnie zaladowane sesje pomiarowe
    const std::vector<PluginSubject::SessionConstPtr>& getCurrentSessions();
    //! dodanie g³ównego elementu (topItem) do drzewa danych w analizach
    //! \param item dodawany element
    void addItemToTree(QTreeWidgetItem* item);
    //! czyszczenie drzewa danych w analizach
    void clearTree();
    //! Pobiera menu kontekstowe dla wybranego elementu drzewa analiz
    //! \param parent rodzic tworzonego menu
    //! \param helper item drzewa, dla którego tworzone jest menu
    //! \return utworzone menu
    QMenu* getContextMenu( QWidget* parent, HmmTreeItem* helper );
    //! \return widget (zakladka) z analizami
    const AnalisisWidget* getAnalisis() const { return analisis; }
    //! Tworzy raport, zakladka z raportami staje siê aktywna
    //! \param html tresc raportu w formie ograniczonego przez Qt html-a
    void createRaport( const QString& html );
    //! Ustawienie kontekstu na konkretny wizualizator
    //! \param visWidget wizualizator, dla którego aktywuje siê kontekst
    virtual void setCurrentVisualizerActions(VisualizerWidget * visWidget);

public slots:
    //! odœwie¿enie drzewa danych z zakladki analiz
    void refreshTree();

private slots:
    //! wywo³ywane po nacisnieciu about
    void onAbout();
    //! Po zniszczeniu wizualizatora trzeba go wyrejstrowac z kontekstow
    //! \param visualizer niszczony wizualizator
    void visualizerDestroyed(QObject * visualizer);
    //! kliknieto filtr, trzeba utworzyc drzewo
    //! \param entry filtr, który zosta³ klikniety
    void filterClicked(FilterEntryWidget* entry);
    //! Kliknieto prawym przyciskiem na drzewie, trzeba pokazac menu kontekstowe
    //! \param pos pozycja, w która kliknieto
    void onTreeContextMenu(const QPoint & pos);
    //! Dodanie pixmapy do raportow
    //! \param pixmap dodawana pixmapa
    void addToRaports(const QPixmap& pixmap);
    //! tworzy nowy wizualizator na podstwie wywo³anej akcji
    void createNewVisualizer();
    //! Na podstawie wybranego elementu drzewa analiz tworzy i dodaje wizualizator w nowej zakladce
    void createVisualizerInNewSet();
    //! Na podstawie wybranego elementu drzewa analiz tworzy i dodaje wizualizator w ustalonym miejscu
    void removeFromVisualizer();
    //! Tworzy wizualizator z wszystkimi seriami danego typu wystepujacymi w sesji
    void allFromSession();
    //! Tworzy wizualizator 2D z wszystkimi seriami X danego typu wystepujacymi w sesji
    void allXFromSession();
    //! Tworzy wizualizator 2D z wszystkimi seriami Y danego typu wystepujacymi w sesji
    void allYFromSession();
    //! Tworzy wizualizator 2D z wszystkimi seriami Z danego typu wystepujacymi w sesji
    void allZFromSession();
    //! Tworzy wizualizator 2D z wszystkimi seriami(o konkretnej nazwie) wystepujacymi w sesji, które zosta³y znormalizowane w kontekscie krokow lewej nogi
    void allLeftNormalized();
    //! Tworzy wizualizator 2D z wszystkimi seriami(o konkretnej nazwie) wystepujacymi w sesji, które zosta³y znormalizowane w kontekscie krokow prawej nogi
    void allRightNormalized();
    //! Tworzy wizualizator 2D na podstawie danych (przypisanych do elementu drzewa analiz), które zosta³y znormalizowane w kontekscie krokow lewej nogi
    void normalizedLeftChart();
    //! Tworzy wizualizator 2D na podstawie danych (przypisanych do elementu drzewa analiz), które zosta³y znormalizowane w kontekscie krokow prawej nogi
    void normalizedRightChart();
    //! Na podstawie elementu drzewa usuwa zwi¹zane z nim serie danych z wszystkich wizualizatorow
    void removeFromAll();
    //! Na podstawie elementu drzewa dodaje zwi¹zane z nim serie do podswietlonego wizualizatora
    void addToVisualizer();
    //! Podswietla wizualizator na podstawie obiektu ContextAction
    void menuHighlightVisualizer(QAction* action = nullptr);
    //! zmieni³a siê aktywnosc jednej z grupy filtrow, mo¿e byæ aktywna tylko jedna
    //! \param active czy grupa jest aktywna, jeœli tak to wszystkie pozostale staja siê nieaktywne
    void filterGroupActivated(bool active);
    //! zosta³ klikniety przycisk z g³ównym kontekstem aplikacji (Dane, analizy, ...)
    //! \param checked 
    void onToolButton(bool checked);

private:
    //! Timeline staje siê widoczny, o ile ju¿ nie jest
    void showTimeline();
    //! Czy pod element drzewa podpiête s¹ jakieœ dane
    //! \param item badany element
    //! \return 
    bool isDataItem(QTreeWidgetItem * item);
    //! Tworzy zakladke z filtrami
    void createFilterTabs();
    //! Tworzy zakladke z filtrami (g³ówne filtry)
    void createFilterTab1();
    //! Tworzy zakladke z filtrami (filtry dla jednostek chorobowych)
    void createFilterTab2();
    //! metoda przechodzi przez cala kolekcje i usuwa te wpisy, których weak pointery przesta³y ju¿ istnieæ
    //! \param multimap sprawdzana kolekcja
    void dropUnusedElements(std::multimap<TreeItemHelperPtr, DataItemDescription>& multimap);
    //! Na podstawie wybranego elementu drzewa analiz tworzy i dodaje wizualizator w ustalonym miejscu
    //! \param hmmItem wybrany item, na podstwie którego tworzony jest wizualizator
    //! \param dockSet set, do którego ma byæ dodany element, jeœli jest nullptr to wizualizator dodawany jest tam, gdzie jest miejsce
    //! \return utworzony dockWidget z wizualizatorem
    VisualizerWidget* createAndAddDockVisualizer( HmmTreeItem* hmmItem, EDRDockWidgetSet* dockSet);
    //! wywo³ywane, gdy jakis widget dostaje focusa.
    //! wykorzystywane na potrzeby kontekstow flexi bara
    //! Hack - dodatkowo podpina siê pod to okienko summary
    //! \param widget widget, który dostaje focusa
    void activateContext(QWidget * widget);
    //! wywo³ywane, gdy obs³ugiwany widget nie ma ju¿ focusa
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
    //! \param dockSet set, do którego ma byæ dodany element, jeœli jest nullptr to wizualizator dodawany jest tam, gdzie jest miejsce
    void createNewVisualizer( HmmTreeItem* item, EDRDockWidgetSet* dockSet = nullptr );
    //! Tworzy wizualizator 2D z wszystkimi seriami(o konkretnej nazwie) wystepujacymi w sesji, które zosta³y znormalizowane w kontekscie krokow
    //! \param helper element, dla którego zostanie utworzony wizualizator
    //! \param context kontekst krokow (lewy, prawy)
    void createNormalizedFromAll( NewVector3ItemHelperPtr helper, c3dlib::C3DParser::IEvent::Context context );
    //! Tworzy wizualizator 2D z wszystkimi seriami(o konkretnej nazwie) wystepujacymi w sesji, które zosta³y znormalizowane w kontekscie krokow
    //! \param helper element, dla którego zostanie utworzony wizualizator
    //! \param context kontekst krokow (lewy, prawy)
    void createNormalized( NewVector3ItemHelperPtr helper, c3dlib::C3DParser::IEvent::Context context );
    //! Tworzy wizualizator 2D z wszystkimi seriami(o konkretnej nazwie i konkretnym kanale) wystepujacymi w sesji
    //! \param helper element, dla którego zostanie utworzony wizualizator
    //! \param channelNo numer kana³u (0 - X, 1 - Y, 2 - Z)
    void allTFromSession( NewVector3ItemHelperPtr helper, int channelNo );
    //! Usuwa serie z wizualizatora
    //! \param action akcja zawierajaca dane o seriach i wizualizatorze
    //! \param once czy usun¹æ z wszystkich, czy tylko ze wskazanego wizualizatora
    void removeFromVisualizers( ContextAction* action, bool once );
    //! podswietla wizualizator zolta ramka, inne traca podswietlenie
    //! \param visualizer wizualizator do podswietlenia
    void highlightVisualizer( const VisualizerPtr& visualizer  );
    
	typedef boost::bimap<QWidget*, boost::bimaps::multiset_of<QWidget*>> DerrivedContextWidgets;
	typedef std::pair<AppUsageContextPtr, QWidget*> ContextState;

private:
    //! kolekcja z aktualnie obs³ugiwanymi sesjami
    std::vector<PluginSubject::SessionConstPtr> currentSessions;
    // TODO, tu jest blad, obiekt jest zawsze nullem
    VisualizerWidget* currentVisualizer;    
    //! gorny widget aplikacji gdzie trafiaja dock Widgety
    EDRDockWidgetManager* topMainWindow;
    //! dolny widget aplikacji na timeline
    QMainWindow* bottomMainWindow;
    //! widget, gdzie trafiaja filtry analiz
    std::vector<DataFilterWidget*> dataFilterWidgets;
    //! struktura z informacjami o stworzonych wizualizatorach, ich seriach oraz z którego elementu powsta³y
    std::multimap<TreeItemHelperPtr, DataItemDescription> items2Descriptions;
    //! widget z analizami
    AnalisisWidget* analisis;
    //! zakladka z danymi
    QWidget* data;
    //! zakladka z operacjami (konsola)
    QWidget* operations;
    //! zakladka z raportami
    TextEdit* raports;
    //! 
    FlexiTabWidget * flexiTabWidget;
    //!
    //FlexiTabWidget::GUIID visualizerGroupID;
    //! mapa [przycisk -> zakladka]
    std::map<QWidget*, QWidget*> button2TabWindow;
    //!
	std::map<QWidget*, ContextState> contextStates;
    //! aktywny przycisk oznaczajacy aktywna zakladke (dane, analizy, operacje... )
    QToolButton* currentButton;
    //! obiekt informuje o zmianie danych w data managerze
    DataObserverPtr dataObserver;
    //! kontekst wizualizatorow
    AppUsageContextPtr visualizerUsageContext;
    //! kontekst drzewa danych w analizach
    HMMTreeItemUsageContextPtr treeUsageContext;
    //! kontekst dla miniaturek raportow
    RaportsThumbnailsContextPtr raportsThumbnailsContext;
    //! kontekst dla kontrolki TextEdit z raportami
    RaportsTabContextPtr raportsTabContext;
    //! kontekst dla zakladki z danymi
	AppUsageContextPtr dataContext;
    //! kontekst dla zakladki z analizami
	AppUsageContextPtr analisisContext;
    //! kontekst dla zakladki z raportami
	AppUsageContextPtr reportsContext;
    //! grupuje konteksty zakladek
	std::set<QWidget*> plainContextWidgets;
    //! 
	DerrivedContextWidgets derrivedContextWidgets;
    //! filtruje zmiany focusow widgetow, obsluguje zdarzenia dla kontekstow
	ContextEventFilter* contextEventFilter;
    //! okienko summary (podsumowanie, informacje o wybranch danych)
    SummaryWindowPtr summaryWindow;
    //! kontrolek okienka z podsumowaniami
    SummaryWindowController* summaryWindowController;
    //! zarz¹dza odœwie¿aniem drzewa danych w analizach
    TreeRefresher treeRefresher;
};


#endif // TOOLBOXMAIN_H
