#ifndef HMM_TOOLBOXMAIN_H
#define HMM_TOOLBOXMAIN_H

#include <core/PluginCommon.h>
#include <plugins/c3d/C3DChannels.h>
#include <plugins/newTimeline/ITimelineService.h>
#include <plugins/video/Wrappers.h>
#include <plugins/kinematic/Wrappers.h>
#include <plugins/subject/Motion.h>
#include "plugins/chart/ChartVisualizer.h"
#include <QtGui/QFrame>
#include "MainWindow.h"
#include "TreeItemHelper.h"
#include "VisualizerWidget.h"
#include "ui_toolboxmaindeffile.h"
#include <stack>
#include <timelinelib/IChannel.h>

class EDRDockWidgetManager;

class HmmMainWindow : public core::MainWindow, private Ui::HMMMain
{
    Q_OBJECT

private:

    struct DataItemDescription{
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
	//! Natywne usuniêcie opcji z menu.
	virtual void onRemoveMenuItem( const std::string& path ) {}

	virtual void init( core::PluginLoader* pluginLoader );

private slots:
	void onOpen();
	void setFont(int size);
	void setTop(int size);
	void setBottom(int size);
	void onTreeItemClicked(QTreeWidgetItem *item, int column);
    void visualizerFocusChanged(bool focus);
    void onTreeContextMenu(const QPoint & pos);
    void createNewVisualizer();

private:

    bool isDataItem(QTreeWidgetItem * item);

	void extractScalarChannels(VectorChannelConstPtr v, ScalarChannelPtr& x, ScalarChannelPtr& y, ScalarChannelPtr& z);

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

	template <class Channel, class CollectionPtr>
	void tryAddVectorToTree(MotionPtr motion, CollectionPtr collection, const std::string& name, QIcon* childIcon, QTreeWidgetItem* parentItem, bool createContainerItem = true ) 
	{
		if (collection) {
            std::vector<ObjectWrapperPtr> wrappers;
            for (int i = 0; i < collection->getNumChannels(); i++) {
                ObjectWrapperPtr wrapper = ObjectWrapper::create<VectorChannel>();
                wrapper->set(boost::dynamic_pointer_cast<VectorChannel>(collection->getChannel(i)));

                static int number = 0;
                std::string name = "Serie_" + boost::lexical_cast<std::string>(number);
                wrapper->setName(name);
                wrapper->setSource(name);
                wrappers.push_back(wrapper);
            }
			QTreeWidgetItem* collectionItem;
            if (createContainerItem) {
                collectionItem = new QTreeWidgetItem();
			    collectionItem->setText(0, tr(name.c_str()));	
			    parentItem->addChild(collectionItem);
            } else {
                collectionItem = parentItem;
            }
			int count = wrappers.size();
			for (int i = 0; i < count; i++) {	
                VectorChannelPtr c = wrappers[i]->get();
				QTreeWidgetItem* channelItem = new QTreeWidgetItem();
				if (childIcon) {
					channelItem->setIcon(0, *childIcon);							
				}				
				channelItem->setText(0, c->getName().c_str());			
				collectionItem->addChild(channelItem);		
                item2Helper[channelItem] = TreeItemHelperPtr(new Vector3ItemHelper(wrappers[i]));
			}	
		}
	}

private:
    std::map<QTreeWidgetItem*, TreeItemHelperPtr> item2Helper;


	VisualizerWidget* currentVisualizer;
    QMainWindow* pane;
	
    EDRDockWidgetManager* topMainWindow;
    QMainWindow* bottomMainWindow;

	QTreeWidget* treeWidget;

    std::map<timeline::IChannelConstPtr, VisualizerWidget*> channelToVisualizer;

    ItemDoubleClick itemClickAction;

    //Zarz¹dzanie widokiem
    //! Zbiór ju¿ dostêpnych, pustych wizualizatorów (ogólne, do specjalizacji)
    std::set<VisualizerWidget*> emptyVisualizersPool;
    //! Zbiór u¿ywanych wizualizatorów
    std::vector<VisualizerWidget*> currentVisualizers;

    std::map<QTreeWidgetItem*, DataItemDescription> items2Descriptions;

    QTreeWidgetItem * currentItem;
};

#endif // TOOLBOXMAIN_H
