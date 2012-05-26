#ifndef HMM_TOOLBOXMAIN_H
#define HMM_TOOLBOXMAIN_H

#include <boost/bimap.hpp>
#include <boost/bimap/multiset_of.hpp>
#include <core/PluginCommon.h>
#include <core/EDRTitleBar.h>
#include <core/IVisualizer.h>
#include <plugins/c3d/C3DChannels.h>
#include <plugins/subject/ISubjectService.h>
#include <plugins/newTimeline/ITimelineService.h>
#include <plugins/video/Wrappers.h>
#include <plugins/kinematic/Wrappers.h>
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
#include "TreeBuilder.h"
#include "SummaryWindow.h"
#include "HmmContexts.h"
 #include <QtGui/QMessageBox>

class EDRDockWidgetManager;
class DataFilterWidget;
class AnalisisWidget;
class EDRDockWidgetSet;
class FilterEntryWidget;


class ContextAction : public QAction
{
    Q_OBJECT;
public:
    ContextAction(HmmTreeItem* itemHelper, QObject* parent, VisualizerPtr vis = VisualizerPtr(), EDRDockWidgetSet* set = nullptr) : 
      item(itemHelper), QAction(parent), visualizer(vis), set(set){}

public:
    HmmTreeItem* getTreeItem() const { return item; }
    VisualizerPtr getVisualizer() const { return visualizer.lock(); }
    EDRDockWidgetSet* getDockSet() const { return set; }

private:
    HmmTreeItem* item;
    VisualizerWeakPtr visualizer;
    EDRDockWidgetSet* set;
};

class TextEdit;

class ContextEventFilter;

class TreeRefresher
{
public:
	TreeRefresher() : 
	  preventRefresh(false), 
		  needRefresh(false),
		  tree(nullptr)
	  {}

	  virtual ~TreeRefresher() {}
	  bool getPreventRefresh() const { return preventRefresh; }
	  void setPreventRefresh(bool val) { 
		  preventRefresh = val; 
		  if (!val && needRefresh) {
			  UTILS_ASSERT(tree);
			  needRefresh = false;
			  std::vector<PluginSubject::SessionConstPtr> sessions = core::queryDataPtr(DataManager::getInstance(), false);
			  actualRefresh(tree, sessions);
			  tree = nullptr;
		  }
	  }

	  void refresh(QTreeWidget* tree) {
		  if (preventRefresh) {
			  this->tree = tree;
			  needRefresh = true;
		  } else {
			  std::vector<PluginSubject::SessionConstPtr> sessions = core::queryDataPtr(DataManager::getInstance(), false);
			  actualRefresh(tree, sessions);
		  }
	  }
private:
	void actualRefresh(QTreeWidget* tree, const std::vector<PluginSubject::SessionConstPtr>& sessions) {

		QMessageBox message;
		message.setWindowTitle(QObject::tr("Refreshing analysis data"));
		message.setText(message.windowTitle());
		message.setWindowFlags(Qt::CustomizeWindowHint);
		message.setStandardButtons(0);

		message.setCursor(Qt::WaitCursor);
		message.show();
		QApplication::processEvents();

		try{

			QTreeWidgetItem* item = TreeBuilder::createTree(QObject::tr("Active Data"), sessions);

			tree->clear();
			tree->addTopLevelItem(item);
			item->setExpanded(true);

			QFont font = item->font(0);
			font.setPointSize(12);
			item->setFont(0, font);

			for (int i = 0; i < item->childCount(); i++) {
				QTreeWidgetItem* child = item->child(i);
				child->setExpanded(true);

				QFont font = item->font(0);
				font.setPointSize(14);
				item->setFont(0, font);
			}

		}catch(...){

		}

		message.setCursor(Qt::ArrowCursor);
	}
	bool preventRefresh;
	bool needRefresh;
	QTreeWidget* tree;
};

class HmmMainWindow : public core::MainWindow, private Ui::HMMMain, protected IAppUsageContextManager
{
    Q_OBJECT

private:

	friend class ContextEventFilter;
    friend class SummaryWindowController; //tymczasowo

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

    const std::vector<PluginSubject::SessionConstPtr>& getCurrentSessions();
    void addItemToTree(QTreeWidgetItem* item);
    void clearTree();
    QMenu* getContextMenu( QWidget* parent, HmmTreeItem* helper );
    const AnalisisWidget* getAnalisis() const { return analisis; }
    void createRaport( const QString& html );
    virtual void setCurrentVisualizerActions(VisualizerWidget * visWidget);

public slots:
    void refreshTree();

private slots:

	void onAbout();

	void activateContext(QWidget * widget);
	void deactivateContext(QWidget * widget);

    //void onFocusChange(QWidget * oldWidget, QWidget * newWidget);
    void visualizerDestroyed(QObject * visualizer);
    void filterClicked(FilterEntryWidget* entry);
    VisualizerWidget* createDockVisualizer(const VisualizerPtr & visualizer);

	void registerVisualizerContext( EDRTitleBar * titleBar, VisualizerWidget* visualizerDockWidget, const VisualizerPtr & visualizer );

	VisualizerWidget* createAndAddDockVisualizer( HmmTreeItem* hmmItem, EDRDockWidgetSet* dockSet);	

    //void addSeriesToTimeline(const std::vector<core::VisualizerTimeSeriePtr> &series, const QString &path, const VisualizerPtr &visualizer );
    void onTreeContextMenu(const QPoint & pos);

    void addToRaports(const QPixmap& pixmap);
    

    void createNewVisualizer();

    void createNewVisualizer( HmmTreeItem* item, EDRDockWidgetSet* dockSet = nullptr );

    void createVisualizerInNewSet();
    void removeFromVisualizer();

    void allFromSession();
    void allXFromSession();
    void allYFromSession();
    void allZFromSession();
    void allLeftNormalized();
    void allRightNormalized();
    void normalizedLeftChart();
    void normalizedRightChart();

    void createNormalizedFromAll( NewVector3ItemHelperPtr helper, c3dlib::C3DParser::IEvent::Context context );


    void createNormalized( NewVector3ItemHelperPtr helper, c3dlib::C3DParser::IEvent::Context context );

    void allTFromSession( NewVector3ItemHelperPtr helper, int channelNo );


    void removeFromVisualizers( ContextAction* action, bool once );

    void removeFromAll();
    void addToVisualizer();
    //void highlightVisualizer();
    void menuHighlightVisualizer(QAction* action = nullptr);

    void highlightVisualizer( const VisualizerPtr& visualizer  );

    void filterGroupActivated(bool active);
    void onToolButton(bool checked);

private:
    void showTimeline();
    bool isDataItem(QTreeWidgetItem * item);
    void extractScalarChannels(VectorChannelConstPtr v, ScalarChannelPtr& x, ScalarChannelPtr& y, ScalarChannelPtr& z);
    void createFilterTabs();
    void createFilterTab1();
    void createFilterTab2();
    void dropUnusedElements(std::multimap<TreeItemHelperPtr, DataItemDescription>& multimap);
    
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

	typedef boost::bimap<QWidget*, boost::bimaps::multiset_of<QWidget*>> DerrivedContextWidgets;
	typedef std::pair<AppUsageContextPtr, QWidget*> ContextState;

private:
    std::vector<PluginSubject::DataFilterPtr> filters;
    std::vector<PluginSubject::SessionConstPtr> currentSessions;
    VisualizerWidget* currentVisualizer;

    PluginSubject::ISubjectService * subjectService;

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

    std::multimap<TreeItemHelperPtr, DataItemDescription> items2Descriptions;

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
	std::map<QWidget*, ContextState> contextStates;
    QToolButton* currentButton;
    DataObserverPtr dataObserver;

    std::list<VisualizerWidget*> currentVisualizerWidgets;

    AppUsageContextPtr visualizerUsageContext;
    HMMTreeItemUsageContextPtr treeUsageContext;
    RaportsThumbnailsContextPtr raportsThumbnailsContext;
    RaportsTabContextPtr raportsTabContext;
	AppUsageContextPtr dataContext;
	AppUsageContextPtr analisisContext;
	AppUsageContextPtr reportsContext;

	std::set<QWidget*> plainContextWidgets;
	DerrivedContextWidgets derrivedContextWidgets;
	ContextEventFilter * contextEventFilter;
    SummaryWindowPtr summaryWindow;
    SummaryWindowController* summaryWindowController;

    TreeRefresher treeRefresher;
};

class ContextEventFilter : public QObject
{
	Q_OBJECT

public:

	ContextEventFilter(HmmMainWindow * mainWindow) : mainWindow(mainWindow) {}
	virtual ~ContextEventFilter() {}

	void registerPermamentContextWidget(QWidget * widget)
	{
		permamentWidgets.insert(widget);
		widget->installEventFilter(this);
	}

	void registerClosableContextWidget(QWidget * widget)
	{
		closableWidgets.insert(widget);
	}

	void unregisterPermamentContextWidget(QWidget * widget)
	{
		permamentWidgets.erase(widget);
	}

	void unregisterClosableContextWidget(QWidget * widget)
	{
		closableWidgets.erase(widget);
	}

protected:

	bool eventFilter(QObject *obj, QEvent *event)
	{
		auto w = (QWidget*)obj;
		auto eType = event->type();
		switch(event->type())
		{
		case QEvent::Close:
			{
				auto it = closableWidgets.find(w);
				if(it != closableWidgets.end()){
					mainWindow->deactivateContext(w);
				}
			}
			break;

		case QEvent::MouseButtonPress:
			{
				auto it = closableWidgets.find(w);
				if(it != closableWidgets.end()){
					mainWindow->activateContext(w);
				}
			}
			break;

		case QEvent::FocusIn:
			{
				auto it = permamentWidgets.find(w);
				if(it != permamentWidgets.end()){
					mainWindow->activateContext(w);
				}
			}
			break;
		}

			return QObject::eventFilter(obj, event);
	}

private:
	HmmMainWindow * mainWindow;

	std::set<QWidget*> permamentWidgets;
	std::set<QWidget*> closableWidgets;
};

#endif // TOOLBOXMAIN_H
