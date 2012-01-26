#ifndef HMM_TOOLBOXMAIN_H
#define HMM_TOOLBOXMAIN_H

#include <core/PluginCommon.h>
#include <core/IVisualizer.h>
#include <plugins/c3d/C3DChannels.h>
#include <plugins/subject/ISubjectService.h>
#include <plugins/newTimeline/ITimelineService.h>
#include <plugins/video/Wrappers.h>
#include <plugins/kinematic/Wrappers.h>
//#include <plugins/subject/Motion.h>
#include "plugins/chart/ChartVisualizer.h"
#include <core/SubjectDataFilters.h>
#include <QtGui/QFrame>
#include "MainWindow.h"
#include "TreeItemHelper.h"
#include "VisualizerWidget.h"
#include "ui_toolboxmaindeffile.h"
#include <stack>
#include <timelinelib/IChannel.h>
#include "FlexiTabWidget.h"
#include "IAppUsageContextManager.h"
#include <QtGui/QToolBox>

#include "HmmContexts.h"

class EDRDockWidgetManager;
class DataFilterWidget;
class AnalisisWidget;
class EDRDockWidgetSet;


class ContextAction : public QAction
{
    Q_OBJECT;
public:
    ContextAction(TreeItemHelper* itemHelper, QObject* parent, VisualizerPtr vis = VisualizerPtr(), EDRDockWidgetSet* set = nullptr) : 
      helper(itemHelper), QAction(parent), visualizer(vis), set(set){}

public:
    TreeItemHelper* getItemHelper() const { return helper; }
    VisualizerPtr getVisualizer() const { return visualizer.lock(); }
    EDRDockWidgetSet* getDockSet() const { return set; }

private:
    TreeItemHelper* helper;
    VisualizerWeakPtr visualizer;
    EDRDockWidgetSet* set;
};

class TextEdit;

    


class HmmMainWindow : public core::MainWindow, private Ui::HMMMain, protected IAppUsageContextManager
{
    Q_OBJECT

private:

private:
    //! Obserwuje Memory data manager. Jezeli dane sie zmienia, to odswiezone zostanie drzewo.
    class DataObserver : public utils::Observer<core::IMemoryDataManager>
    {
    public:
        DataObserver(HmmMainWindow* hmm) : hmm(hmm), motionsCount(0) {}
        virtual void update( const core::IMemoryDataManager * subject );

    private:
        HmmMainWindow* hmm;
        int motionsCount;
    };
    typedef core::shared_ptr<DataObserver> DataObserverPtr;

    struct DataItemDescription 
    {
        DataItemDescription(VisualizerWeakPtr visualizer, const std::vector<core::VisualizerTimeSeriePtr>& series, VisualizerWidget* widget) : 
            visualizer(visualizer),
            visualizerWidget(widget) 
        {
            // konwersja na weak ptr.
            for (auto it = series.begin(); it != series.end(); it++) {
                this->series.push_back(*it);
            }
        }
        std::vector<core::VisualizerTimeSerieWeakPtr> series;
        VisualizerWeakPtr visualizer;
        VisualizerWidget* visualizerWidget;
    };

public:
	HmmMainWindow();
    virtual ~HmmMainWindow();

public:
	//! Natywne dodanie opcji do menu.
	virtual void onAddMenuItem( const std::string& path, bool checkable, bool initialState ) {}
	//! Natywne usuniêcie opcji z menu.
	virtual void onRemoveMenuItem( const std::string& path ) {}

	virtual void init( core::PluginLoader* pluginLoader, core::IManagersAccessor * managersAccessor );

    const std::vector<SessionConstPtr>& getCurrentSessions();
    void addItemToTree(QTreeWidgetItem* item);
    void clearTree();
    QMenu* getContextMenu( QWidget* parent, TreeItemHelper* helper );
    const AnalisisWidget* getAnalisis() const { return analisis; }
    void createRaport( const QString& html );
    virtual void setCurrentVisualizerActions(VisualizerWidget * visWidget);

private slots:
    void onFocusChange(QWidget * oldWidget, QWidget * newWidget);
    void visualizerDestroyed(QObject * visualizer);

	//void onOpen();
	void onTreeItemClicked(QTreeWidgetItem *item, int column);

    VisualizerWidget* createDockVisualizer( TreeItemHelper* hmmItem );

    void addSeriesToTimeline(const std::vector<core::VisualizerTimeSeriePtr> &series, const QString &path, const VisualizerPtr &visualizer );

    void onTreeContextMenu(const QPoint & pos);

    void addToRaports(const QPixmap& pixmap);
    

    void createNewVisualizer();
    void createVisualizerInNewSet();
    void removeFromVisualizer();
    void addToVisualizer();
    //void highlightVisualizer();
    void menuHighlightVisualizer(QAction* action = nullptr);

    void highlightVisualizer( const VisualizerPtr& visualizer  );

    void filterGroupActivated(bool active);
    void onToolButton(bool checked);

private:
    void showTimeline();
    bool isDataItem(QTreeWidgetItem * item);
    //void createTree(const std::vector<SessionPtr>& sessions);
	void extractScalarChannels(VectorChannelConstPtr v, ScalarChannelPtr& x, ScalarChannelPtr& y, ScalarChannelPtr& z);
    void createFilterTabs();
    void createFilterTab1();
    void createFilterTab2();
    void refreshTree();
    void dropUnusedElements(std::multimap<TreeItemHelper*, DataItemDescription>& multimap);
    
    class ItemDoubleClick
    {
    public:

        ItemDoubleClick() : window(0) {}

        void operator()(const timeline::IChannelConstPtr & channel){
            auto it = window->channelToVisualizer.find(channel);
            if(it != window->channelToVisualizer.end()){
                it->second->setVisible(!it->second->isVisible());
            }
        }

        void setMainWindow(HmmMainWindow * window)
        {
            this->window = window;
        }

    private:
        HmmMainWindow * window;
    };

    friend class ItemDoubleClick;

private:
    std::vector<DataFilterPtr> filters;
    std::vector<SessionConstPtr> currentSessions;
	VisualizerWidget* currentVisualizer;

    ISubjectService * subjectService;

    QMainWindow* pane;
	
    EDRDockWidgetManager* topMainWindow;
    QMainWindow* bottomMainWindow;

	std::vector<DataFilterWidget*> dataFilterWidgets;
    std::map<timeline::IChannelConstPtr, VisualizerWidget*> channelToVisualizer;

    ItemDoubleClick itemClickAction;

    //Zarz¹dzanie widokiem
    //! Zbiór ju¿ dostêpnych, pustych wizualizatorów (ogólne, do specjalizacji)
    std::set<VisualizerWidget*> emptyVisualizersPool;
    //! Zbiór u¿ywanych wizualizatorów
    std::vector<VisualizerWidget*> currentVisualizers;

    std::multimap<TreeItemHelper*, DataItemDescription> items2Descriptions;

    QTreeWidgetItem * currentItem;
    AnalisisWidget* analisis;
    
    QWidget* data;
    QWidget* operations;
    TextEdit* raports;

    QWidget* visualizersActionsTab;
    FlexiTabWidget * flexiTabWidget;

    FlexiTabWidget::GUIID visualizerGroupID;
    FlexiTabWidget::GUIID toolsGroupID;

    std::map<QWidget*, QWidget*> button2TabWindow;
    QToolButton* currentButton;
    DataObserverPtr dataObserver;

    std::list<VisualizerWidget*> currentVisualizerWidgets;

    AppUsageContextPtr visualizerUsageContext;
    HMMTreeItemUsageContextPtr treeUsageContext;
    RaportsThumbnailsContextPtr raportsThumbnailsContext;
    RaportsTabContextPtr raportsTabContext;
};

#endif // TOOLBOXMAIN_H
