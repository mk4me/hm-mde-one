#ifndef HMM_TOOLBOXMAIN_H
#define HMM_TOOLBOXMAIN_H

#include <plugins/c3d/C3DChannels.h>
#include <plugins/newTimeline/ITimelineService.h>
#include <plugins/video/Wrappers.h>
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
public:
	HmmMainWindow();

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
    void visualizerGainedFocus();

private:

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

                TimelinePtr timeline = core::queryServices<ITimelineService>(core::getServiceManager());
                if(timeline != nullptr) {
                    timeline->setChannelTooltip(path.toStdString(), "test Tooltip");
                    timeline::IChannelConstPtr channel(core::dynamic_pointer_cast<const timeline::IChannel>(serie));
                    if(channel != nullptr){
                        channelToVisualizer[channel] = visu;
                        timeline->setOnChannelDblClick(itemClickAction);
                    }
                }

                vis->getWidget()->setFocusProxy(visu);

                connect(visu, SIGNAL(focuseGained()), this, SLOT(visualizerGainedFocus()));
    
                visu->setMinimumSize(250, 100);

                topMainWindow->addDockWidget(Qt::TopDockWidgetArea, visu, Qt::Horizontal);
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

                TimelinePtr timeline = core::queryServices<ITimelineService>(core::getServiceManager());
                if(timeline != nullptr) {
                    timeline->setChannelTooltip(path.toStdString(), "test Tooltip");
                    timeline::IChannelConstPtr channel(core::dynamic_pointer_cast<const timeline::IChannel>(serie));
                    if(channel != nullptr){
                        channelToVisualizer[channel] = visu;
                        timeline->setOnChannelDblClick(itemClickAction);
                    }
                }

                vis->getWidget()->setFocusProxy(visu);

                connect(visu, SIGNAL(focuseGained()), this, SLOT(visualizerGainedFocus()));

			
			    /*topMainWindow->setUpdatesEnabled(false);
			    if (currentVisualizer) {
				    topMainWindow->layout()->removeWidget(currentVisualizer);
				    delete currentVisualizer;
			    }*/
			    //currentVisualizer = visu;
			    //topMainWindow->layout()->addWidget(visu);            
                topMainWindow->addDockWidget(Qt::TopDockWidgetArea, visu, Qt::Horizontal);
			    //topMainWindow->setUpdatesEnabled(true);
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
			}	
		}
	}

private:
	std::map<QTreeWidgetItem*, ScalarChannelConstPtr> item2ScalarMap;
	std::map<QTreeWidgetItem*, MarkerCollectionConstPtr> item2Markers;
    std::map<QTreeWidgetItem*, EventsCollectionConstPtr> item2Events;
	std::map<QTreeWidgetItem*, VectorChannelConstPtr> item2Vector;
	std::map<QTreeWidgetItem*, VideoChannelConstPtr> item2Video;
	
    VisualizerWidget* currentVisualizer;

    QMainWindow* topMainWindow;
    QMainWindow* bottomMainWindow;

	QTreeWidget* treeWidget;

    std::map<timeline::IChannelConstPtr, VisualizerWidget*> channelToVisualizer;

    ItemDoubleClick itemClickAction;

    osg::ref_ptr<osgText::Text> chartTextPrototype;
};

#endif // TOOLBOXMAIN_H
