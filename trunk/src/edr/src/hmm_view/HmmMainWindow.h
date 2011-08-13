#ifndef HMM_TOOLBOXMAIN_H
#define HMM_TOOLBOXMAIN_H

#include <plugins/c3d/C3DChannels.h>
#include <QtGui/QFrame>
#include "MainWindow.h"
#include "ui_toolboxmaindeffile.h"

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
			VisualizerWidget* visualizerWidget = new VisualizerWidget(vis);
            visualizerWidget->setAllowedAreas(Qt::RightDockWidgetArea);
            visualizerWidget->setStyleSheet(styleSheet());
            visualizerWidget->setTitleBarVisible(false);

			vis->getWidget()->setFocusProxy(visualizerWidget);

			vis->createSerie(wrapper, prefix + "seria testowa");

            connect(visualizerWidget, SIGNAL(focuseGained()), this, SLOT(visualizerGainedFocus()));
			
			/*pane->setUpdatesEnabled(false);
			if (currentVisualizer) {
				pane->layout()->removeWidget(currentVisualizer);
				delete currentVisualizer;
			}*/
			//currentVisualizer = visu;
			//pane->layout()->addWidget(visu);
            pane->addDockWidget(Qt::RightDockWidgetArea, visualizerWidget);
			//pane->setUpdatesEnabled(true);
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
	std::map<QTreeWidgetItem*, VectorChannelConstPtr> item2Vector;
	VisualizerWidget* currentVisualizer;
	//QWidget* pane;
    QMainWindow* pane;
	QTreeWidget* treeWidget;
};

#endif // TOOLBOXMAIN_H
