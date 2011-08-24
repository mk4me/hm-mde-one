#ifndef HMM_TOOLBOXMAIN_H
#define HMM_TOOLBOXMAIN_H

#include <plugins/c3d/C3DChannels.h>
#include <QtGui/QFrame>
#include "MainWindow.h"
#include "ui_toolboxmaindeffile.h"
#include <stack>

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
	template <class T, class Ptr >
	void onClicked( QTreeWidgetItem * item, const std::map<QTreeWidgetItem*, Ptr>& item2Channel )
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
                visu->setStyleSheet(styleSheet());

                visu->setTitleBarVisible(false);

			    //vis->createSerie(wrapper, prefix + "seria testowa");
                vis->createSerie(wrapper, path.toStdString());

                vis->getWidget()->setFocusProxy(visu);

                connect(visu, SIGNAL(focuseGained()), this, SLOT(visualizerGainedFocus()));

			
			    /*pane->setUpdatesEnabled(false);
			    if (currentVisualizer) {
				    pane->layout()->removeWidget(currentVisualizer);
				    delete currentVisualizer;
			    }*/
			    //currentVisualizer = visu;
			    //pane->layout()->addWidget(visu);            
                pane->addDockWidget(Qt::TopDockWidgetArea, visu, Qt::Horizontal);
			    //pane->setUpdatesEnabled(true);
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
	VisualizerWidget* currentVisualizer;
	//QWidget* pane;
    QMainWindow* pane;
	QTreeWidget* treeWidget;
};

#endif // TOOLBOXMAIN_H
