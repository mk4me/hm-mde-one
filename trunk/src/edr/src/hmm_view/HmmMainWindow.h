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
#include "VisualizerWidget.h"
#include "ui_toolboxmaindeffile.h"
#include <stack>
#include <timelinelib/IChannel.h>

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

    template <class T, class Ptr >
    void onClickedScalar( QTreeWidgetItem * item, const std::map<QTreeWidgetItem*, Ptr>& item2Channel)
    {
        // sprawdzanie, czy pod item jest podpiety jakis obiekt
		auto it = item2Channel.find(item);
		if (it != item2Channel.end()) {
            std::stack<QString> pathStack;

            QTreeWidgetItem * pomItem = item;

            while(pomItem != nullptr){
                pathStack.push(pomItem->text(0));
                pomItem = pomItem->parent();
            }

            QString path;

            path += pathStack.top();
            pathStack.pop();

            while(pathStack.empty() == false){
                path += "/";
                path += pathStack.top();
                pathStack.pop();
            }

            if(VisualizerManager::getInstance()->existVisualizerForType(typeid(*(it->second).get())) == true){
			    VisualizerPtr vis = VisualizerManager::getInstance()->createVisualizer(typeid(*(it->second).get()));
                //ObjectWrapperPtr wrapper = ObjectWrapper::create<T>();
                ObjectWrapperPtr wrapper = ObjectWrapper::create<ScalarChannelReaderInterface>();
                //Ptr ()
                wrapper->set(core::const_pointer_cast<ScalarChannelReaderInterface>(core::dynamic_pointer_cast<const ScalarChannelReaderInterface>(it->second)));
                static std::string prefix = typeid(T).name();
                // hack + todo - rozwiazanie problemu z zarejesrowanymi nazwami w timeline
                prefix += "_";
                wrapper->setName(prefix + "nazwa testowa");
                wrapper->setSource(prefix + "Sciezka testowa");
                vis->getOrCreateWidget();
			   
			    VisualizerWidget* visu = new VisualizerWidget(vis);
                visu->setAllowedAreas(Qt::TopDockWidgetArea | Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
                //visu->setStyleSheet(styleSheet());
                visu->setVisualizerIconVisible(false);
                visu->setSplitHVisible(false);
                visu->setSplitVVisible(false);
                visu->setActiveVisualizerSwitch(false);
                visu->setSourceVisible(false);

                visu->setTitleBarVisible(false);

                auto serie = vis->createSerie(wrapper, path.toStdString());

                ChartVisualizer* chart = dynamic_cast<ChartVisualizer*>(vis->getImplementation());

                
                chart->setAutoRefresh(false);

                chart->setLabelPrototype(chartTextPrototype);

                chart->setBackgroundColor(osg::Vec4(1,1,1,1));
                chart->setCursorColor(osg::Vec4(0,0,0,1));
                chart->setAxisesColor(osg::Vec4(0,0,0,1));
                chart->setFrameColor(osg::Vec4(0,0,0,1));
                chart->setShowFrame(true);

                chart->setGridColor(osg::Vec4(0.2,0.2,0.2,1));

                chart->setShowGridX(true);
                chart->setShowGridY(true);

                chart->setMargin(5);

                chart->setAxisXInside(false);
                chart->setAxisYInside(false);

                chart->setShowAxisX(true);
                chart->setShowAxisY(true);

                chart->setShowingXUnits(true);
                chart->setShowingYUnits(false);

                chart->setShowingXUnitsSeparately(true);
                chart->setShowingYUnitsSeparately(true);

                chart->setShowingTimeInCursor(true);
                chart->setShowingUnitsInCursor(true);

                chart->setShowCursorBackground(true);
                chart->setCursorBackgroundColor(osg::Vec4(1,1,1,0.95));

                chart->setShowTitle(true);

                std::string title;
                title += item->text(0).toStdString();
                title += " [";
                title += it->second->getValueBaseUnit();
                title += "]";
                chart->setTitleText(title);
                chart->setTitleTextSize(chartTextPrototype->getCharacterHeight());
                chart->setTitlePosition(Chart::TOP_GRID_CENTER);

                chart->setAutoRefresh(true);

                ChartVisualizer::ChartVisualizerSerie* chartSerie = dynamic_cast<ChartVisualizer::ChartVisualizerSerie*>(serie.get());

                if(item->text(0).at(0) == 'L' || item->text(0).at(0) == 'l' || item->text(0).at(1) == 'X' || item->text(0).at(1) == 'x'){
                    chartSerie->setColor(osg::Vec4(1,0,0,1));
                }else if(item->text(0).at(0) == 'R' || item->text(0).at(0) == 'r' || item->text(0).at(1) == 'Y' || item->text(0).at(1) == 'y'){
                    chartSerie->setColor(osg::Vec4(0,1,0,1));
                }else{
                    chartSerie->setColor(osg::Vec4(0.5,0,1,1));
                }

                vis->getWidget()->setFocusProxy(visu);

                connect(visu, SIGNAL(focuseChanged(bool)), this, SLOT(visualizerFocusChanged(bool)));
    
                visu->setMinimumSize(250, 100);

                topMainWindow->addDockWidget(Qt::TopDockWidgetArea, visu, Qt::Horizontal);

                TimelinePtr timeline = core::queryServices<ITimelineService>(core::getServiceManager());
                if(timeline != nullptr) {

                    VisualizerChannelPtr channel(new VisualizerChannel(core::dynamic_pointer_cast<IVisualizer::TimeSerieBase>(serie), visu));

                    try{
                        timeline->addChannel(path.toStdString(), channel);
                    }catch(...){
                        LOG_ERROR("Could not add multichannel to timeline!");
                    }
                }

            }else{

                /*TimelinePtr timeline = core::queryServices<TimelineService>(core::getServiceManager());
                if(timeline != nullptr && dynamic_cast<timeline::IChannel*>(serie.get()) != nullptr) {
                    timeline::IChannelPtr channel = core::dynamic_pointer_cast<timeline::IChannel>(serie);
                    timeline->addChannel(name, channel);
                    timelineDataSeries.insert(serie);
                }*/
            }
		}
    }

    template <class T, class Ptr >
    void onClickedOther( QTreeWidgetItem * item, const std::map<QTreeWidgetItem*, Ptr>& item2Channel)
	{
		// sprawdzanie, czy pod item jest podpiety jakis obiekt
		auto it = item2Channel.find(item);
		if (it != item2Channel.end()) {
            std::stack<QString> pathStack;
			/*VisualizerPtr vis = VisualizerManager::getInstance()->createVisualizer(typeid(*(it->second).get()));
			ObjectWrapperPtr wrapper = ObjectWrapper::create<T>();
			wrapper->set(boost::dynamic_pointer_cast<T>(boost::const_pointer_cast<T>(it->second)));
			static std::string prefix = typeid(T).name();
			// hack + todo - rozwiazanie problemu z zarejesrowanymi nazwami w timeline
			prefix += "_";
			wrapper->setName(prefix + "nazwa testowa");
			wrapper->setSource(prefix + "Sciezka testowa");
			vis->getOrCreateWidget();
			VisualizerWidget* visualizerWidget = new VisualizerWidget(vis);
            visualizerWidget->setAllowedAreas(Qt::RightDockWidgetArea);
            visualizerWidget->setStyleSheet(styleSheet());
            visualizerWidget->setTitleBarVisible(false);*/

            QTreeWidgetItem * pomItem = item;

            while(pomItem != nullptr){
                pathStack.push(pomItem->text(0));
                pomItem = pomItem->parent();
            }

            QString path;

            path += pathStack.top();
            pathStack.pop();

            while(pathStack.empty() == false){
                path += "/";
                path += pathStack.top();
                pathStack.pop();
            }

			VisualizerManager* visualizerManager = VisualizerManager::getInstance();
			const T* channel = (it->second).get();
            if(visualizerManager->existVisualizerForType(typeid(*channel)) == true){
			    VisualizerPtr vis = VisualizerManager::getInstance()->createVisualizer(typeid(*(it->second).get()));
                ObjectWrapperPtr wrapper = ObjectWrapper::create<T>();
                //Ptr ()
                wrapper->set(boost::dynamic_pointer_cast<T>(boost::const_pointer_cast<T>(it->second)));
                static std::string prefix = typeid(T).name();
                // hack + todo - rozwiazanie problemu z zarejesrowanymi nazwami w timeline
                prefix += "_";
                wrapper->setName(prefix + "nazwa testowa");
                wrapper->setSource(prefix + "Sciezka testowa");
                vis->getOrCreateWidget();
			   
			    VisualizerWidget* visu = new VisualizerWidget(vis);
                visu->setAllowedAreas(Qt::TopDockWidgetArea | Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
                //visu->setStyleSheet(styleSheet());
                visu->setVisualizerIconVisible(false);
                visu->setSplitHVisible(false);
                visu->setSplitVVisible(false);
                visu->setActiveVisualizerSwitch(false);
                visu->setSourceVisible(false);

                visu->setTitleBarVisible(false);

                auto serie = vis->createSerie(wrapper, path.toStdString());

                vis->getWidget()->setFocusProxy(visu);

                connect(visu, SIGNAL(focuseChanged(bool)), this, SLOT(visualizerFocusChanged(bool)));

                topMainWindow->addDockWidget(Qt::TopDockWidgetArea, visu, Qt::Horizontal);

                auto timeSerie = core::dynamic_pointer_cast<IVisualizer::TimeSerieBase>(serie);

                if(timeSerie != nullptr){

                    TimelinePtr timeline = core::queryServices<ITimelineService>(core::getServiceManager());
                    if(timeline != nullptr) {
                        VisualizerChannelPtr channel(new VisualizerChannel(timeSerie, visu));
                        try{
                            timeline->addChannel(path.toStdString(), channel);
                        }catch(...){
                            LOG_ERROR("Could not add multichannel to timeline!");
                        }
                    }
                }
            }else{

                /*TimelinePtr timeline = core::queryServices<TimelineService>(core::getServiceManager());
                if(timeline != nullptr && dynamic_cast<timeline::IChannel*>(serie.get()) != nullptr) {
                    timeline::IChannelPtr channel = core::dynamic_pointer_cast<timeline::IChannel>(serie);
                    timeline->addChannel(name, channel);
                    timelineDataSeries.insert(serie);
                }*/
            }
		}
	}

	template <class Collection>
	void tryAddVectorToTree( Collection collection, const std::string& name, QIcon* childIcon, QTreeWidgetItem* parentItem ) 
	{
		if (collection) {
			QTreeWidgetItem* collectionItem = new QTreeWidgetItem();
			collectionItem->setText(0, tr(name.c_str()));	
			parentItem->addChild(collectionItem);
			int count = collection->getNumChannels();
			for (int i = 0; i < count; i++) {							
				QTreeWidgetItem* channelItem = new QTreeWidgetItem();
				if (childIcon) {
					channelItem->setIcon(0, *childIcon);							
				}
				auto c = collection->getChannel(i);				
				channelItem->setText(0, c->getName().c_str());			
				collectionItem->addChild(channelItem);						
				item2Vector[channelItem] = 							
					boost::dynamic_pointer_cast<const VectorChannel>(c);
                dataItems.insert(channelItem);
			}	
		}
	}

private:
	std::map<QTreeWidgetItem*, GRFCollectionConstPtr> item2GrfCollection;
	std::map<QTreeWidgetItem*, ScalarChannelConstPtr> item2ScalarMap;
	std::map<QTreeWidgetItem*, MarkerCollectionConstPtr> item2Markers;
    std::map<QTreeWidgetItem*, EventsCollectionConstPtr> item2Events;
	std::map<QTreeWidgetItem*, VectorChannelConstPtr> item2Vector;
	std::map<QTreeWidgetItem*, VideoChannelConstPtr> item2Video;
	std::map<QTreeWidgetItem*, kinematic::JointAnglesCollectionConstPtr> item2Skeleton;
	std::map<QTreeWidgetItem*, MotionPtr> item2Kinematic;

    std::set<QTreeWidgetItem*> dataItems;

	VisualizerWidget* currentVisualizer;
	//QWidget* pane;
    QMainWindow* pane;
	
    QMainWindow* topMainWindow;
    QMainWindow* bottomMainWindow;

	QTreeWidget* treeWidget;

    std::map<timeline::IChannelConstPtr, VisualizerWidget*> channelToVisualizer;

    ItemDoubleClick itemClickAction;

    osg::ref_ptr<osgText::Text> chartTextPrototype;

    //Zarz¹dzanie widokiem
    //! Zbiór ju¿ dostêpnych, pustych wizualizatorów (ogólne, do specjalizacji)
    std::set<VisualizerWidget*> emptyVisualizersPool;
    //! Zbiór u¿ywanych wizualizatorów
    std::vector<VisualizerWidget*> currentVisualizers;

    std::map<QTreeWidgetItem*, DataItemDescription> items2Descriptions;

    QTreeWidgetItem * currentItem;
};

#endif // TOOLBOXMAIN_H
