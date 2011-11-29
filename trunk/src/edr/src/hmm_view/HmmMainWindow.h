#ifndef HMM_TOOLBOXMAIN_H
#define HMM_TOOLBOXMAIN_H

#include <core/PluginCommon.h>
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

class EDRDockWidgetManager;
class DataFilterWidget;
class AnalisisWidget;

class ContextAction : public QAction
{
    Q_OBJECT;
public:
    ContextAction(TreeItemHelper* itemHelper, QObject* parent, VisualizerPtr vis = VisualizerPtr()) : 
      helper(itemHelper), QAction(parent), visualizer(vis) {}

public:
    TreeItemHelper* getItemHelper() const { return helper; }
    VisualizerPtr getVisualizer() const { return visualizer; }

private:
    TreeItemHelper* helper;
    VisualizerPtr visualizer;
};




class HmmMainWindow : public core::MainWindow, private Ui::HMMMain
{
    Q_OBJECT

private:

    struct DataItemDescription {
        core::IVisualizer::SerieBase* serie;
        Visualizer* visualizer;
        VisualizerWidget* wisualizerWidget;
    };

public:
	HmmMainWindow();
    virtual ~HmmMainWindow();

public:
	//! Natywne dodanie opcji do menu.
	virtual void onAddMenuItem( const std::string& path, bool checkable, bool initialState ) {}
	//! Natywne usuni�cie opcji z menu.
	virtual void onRemoveMenuItem( const std::string& path ) {}

	virtual void init( core::PluginLoader* pluginLoader, core::IManagersAccessor * managersAccessor );

    const std::vector<SessionConstPtr>& getCurrentSessions();
    void addItemToTree(QTreeWidgetItem* item);
    void clearTree();

private slots:
    void visualizerDestroyed(QObject * visualizer);

	void onOpen();
	void onTreeItemClicked(QTreeWidgetItem *item, int column);

     VisualizerWidget* createDockVisualizer( TreeItemHelper* hmmItem );

     void addSeriesToTimeline(const std::vector<core::VisualizerTimeSeriePtr> &series, const QString &path, const VisualizerPtr &visualizer );

    void visualizerFocusChanged(bool focus);
    void onTreeContextMenu(const QPoint & pos);
    void createNewVisualizer();
    void addToVisualizer();
    void filterGroupActivated(bool active);
    void onToolButton();

private:
    void showTimeline();
    bool isDataItem(QTreeWidgetItem * item);
    //void createTree(const std::vector<SessionPtr>& sessions);
	void extractScalarChannels(VectorChannelConstPtr v, ScalarChannelPtr& x, ScalarChannelPtr& y, ScalarChannelPtr& z);
    void createFilterTabs();
    void createFilterTab1();
    void createFilterTab2();

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

    //Zarz�dzanie widokiem
    //! Zbi�r ju� dost�pnych, pustych wizualizator�w (og�lne, do specjalizacji)
    std::set<VisualizerWidget*> emptyVisualizersPool;
    //! Zbi�r u�ywanych wizualizator�w
    std::vector<VisualizerWidget*> currentVisualizers;

    std::map<QTreeWidgetItem*, DataItemDescription> items2Descriptions;

    QTreeWidgetItem * currentItem;
    AnalisisWidget* analisis;
    QWidget* tests;
    QWidget* operations;
    QWidget* raports;
    std::map<QWidget*, QWidget*> button2TabWindow;
};

#endif // TOOLBOXMAIN_H