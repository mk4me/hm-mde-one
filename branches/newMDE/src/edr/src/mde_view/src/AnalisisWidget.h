///********************************************************************
//	created:	2011/10/21
//	created:	21:10:2011   11:09
//	filename: 	AnalisisWidget.h
//	author:		Wojciech Kniec
//	
//	purpose:	
//*********************************************************************/

#ifndef HEADER_GUARD_HMM__ANALISISWIDGET_H__
#define HEADER_GUARD_HMM__ANALISISWIDGET_H__

#include <coreui/CoreDockWidgetManager.h>
#include <coreui/CoreDockWidget.h>
#include <coreui/CoreTitleBar.h>
#include <coreui/CoreVisualizerWidget.h>
#include <coreui/DataFilterWidget.h>
#include <corelib/HierarchyHelper.h>
#include "ui_AnalisisWidget.h"
#include "AnalisisModel.h"
#include "IAppUsageContext.h"

class AnalisisTreeWidget;
class AnalysisTreeContextMenu;

//! Klasa jest odpowiedzialna za widok zakładki analiz
class AnalisisWidget : public QWidget, public Ui::AnalisisWidget
{
    Q_OBJECT;
public:
    //! Konstruktor
    //! \param parent rodzic widgeta
    //! \param hmm obiekt widoku HMM 
    //! \param margin margines dla filtrów
    //! \param flags flagi Qt
    AnalisisWidget(AnalisisModelPtr model, QWidget* parent, int margin = 2, Qt::WindowFlags flags = 0);
    virtual ~AnalisisWidget() {}

public:
    ////! \return drzewo danych
    //QTreeWidget* getTreeWidget();
    ////! \return widget przechowujący miniaturki do raportów
    //QWidget* getRaportsThumbnailList();
    ////! dodaje widget z filtrami
    //! \param filter dodawany widget
    void addDataFilterWidget(coreUi::DataFilterWidget* filter);
    //! \return obszar, gdzie laduja wizualizatory i timeline
    QWidget* getArea() { return analisisArea; }
    QTreeView* getTreeView() { return treeView; }
    void registerVisualizerContext(ContextEventFilterPtr contextEventFilter, coreUI::CoreTitleBar * titleBar, coreUI::CoreVisualizerWidget* visualizerDockWidget, const core::VisualizerPtr & visualizer );
    void setContextItems( IAppUsageContextManager* manager, IAppUsageContextPtr parent, QTabWidget * flexiTabWidget );
public Q_SLOTS:
    void createVisualizer( core::IHierarchyDataItemConstPtr treeItem, core::HierarchyHelperPtr helper);
    void addRoot(core::IHierarchyItemPtr root);


private:
    void showTimeline();
    void devideArea();
    QDockWidget* createDockVisualizer(const core::VisualizerPtr & visualizer);
    QDockWidget* createAndAddDockVisualizer( core::IHierarchyDataItemConstPtr treeItem, core::HierarchyHelperPtr helper, coreUI::CoreDockWidgetSet* dockSet);
    coreUI::CoreDockWidget* embeddWidget(QWidget * widget, const QString & windowTitle, Qt::DockWidgetArea allowedAreas, bool permanent);
    
private Q_SLOTS:
    void visualizerDestroyed(QObject * visualizer);
    void onFilterBundleAdded(core::IFilterBundlePtr bundle);
    void onFilterClicked(core::IFilterCommandPtr filter);
    ////! zmienia widoczność filtrów
    ////! \param expand chowa/pokazuje filtry
    //void setFiltersExpanded(bool expand);
    ////! kliknięto w filtr
    ////! \param filter kliknięty
    //void filterClicked(FilterEntryWidget* filter);
    ////! Kliknięto zatwierdź w graficznym konfiguratorze
    //void applyClicked();
    ////! proste przejście do pierwszej zakładki (z drzewem danych)
    //void switchToFirstTab();
    ////! odtwarza drzewo danych 
    ////! \param currentFilter filtr, na podstawie którego odtworzone zostanie drzewo
    //void recreateTree(FilterEntryWidget* currentFilter);

private:
    coreUI::CoreDockWidgetManager* topMainWindow;
    QFrame* bottomMainWindow;
    AnalisisModelPtr model;
    AnalysisTreeContextMenu* contextMenu;
    IAppUsageContextManager* manager;
    IAppUsageContextPtr parent;
    QTabWidget * flexiTabWidget;
    //! szerokość filtru w zakładce
    int filterWidth;
    //! wysokość filtru w zakładce
    int filterHeight;
    //! margines dla filtrów w zakładce
    int margin;
    ////! drzewo danych
    //AnalisisTreeWidget* treeWidget;
    ////! widok HMM
    //HmmMainWindow* hmm;
    ////TODO potrzebne tylko, aby przekazac info między elementami.
    //FilterEntryWidget* currentFilter;
};


//! Akcja przekazuje informację o konkretnie wybranym helperze
class HelperAction : public QAction 
{
    Q_OBJECT;
public:
    HelperAction(core::HierarchyHelperPtr helper, const QIcon& icon, const QString& text, QObject* parent = nullptr) :
        QAction(icon, text, parent),
        helper(helper) 
    {} 
    core::HierarchyHelperPtr getHelper() const { return helper; }
private:
    core::HierarchyHelperPtr helper;
};


//! Pomocnicza klasa obsługująca menu kontekstowe modelu drzewa
//! Elementy drzewa tworzone są z wykorzystaniem "helperów" podpiętych pod element hierarchii danych
class AnalysisTreeContextMenu : public QObject
{
    Q_OBJECT;
public:
    //! 
    //! \param model 
    //! \param widget 
    AnalysisTreeContextMenu(AnalisisModelPtr model, AnalisisWidget* widget);

Q_SIGNALS:
    //! sygnał przekazuje potrzebne dane, do utworzenia wizualizatora.
    void createVisualizer(core::IHierarchyDataItemConstPtr item, core::HierarchyHelperPtr helper);

public Q_SLOTS:
    //! 
    //! \param clickedPoint 
    void contextualMenu(const QPoint& clickedPoint);

private Q_SLOTS:
    //! wywoływane, gdy wybrany został helper
    void onCreateVisualizer();

private:
    //! Model analiz, potrzebny do pobrania danych z hierarchii
    AnalisisModelPtr model;
    //! Widget analiz, przekazuje informacje o kliknięciach 
    AnalisisWidget* widget;
};

#endif
