#ifndef HMM_TOOLBOXMAIN_H
#define HMM_TOOLBOXMAIN_H

#include <plugins/c3d/C3DChannels.h>
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
	//! Natywne usuniÍcie opcji z menu.
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
			VisualizerWidget* visu = new VisualizerWidget(vis);
            visu->setAllowedAreas(Qt::RightDockWidgetArea);
            visu->setStyleSheet(styleSheet());

            visu->setTitleBarVisible(false);

			vis->createSerie(wrapper, prefix + "seria testowa");

            connect(visu, SIGNAL(focuseGained()), this, SLOT(visualizerGainedFocus()));
			
			/*pane->setUpdatesEnabled(false);
			if (currentVisualizer) {
				pane->layout()->removeWidget(currentVisualizer);
				delete currentVisualizer;
			}*/
			//currentVisualizer = visu;
			//pane->layout()->addWidget(visu);
            pane->addDockWidget(Qt::RightDockWidgetArea, visu);
			//pane->setUpdatesEnabled(true);
		}
	}
private:
	std::map<QTreeWidgetItem*, ScalarChannelConstPtr> item2ScalarMap;
	std::map<QTreeWidgetItem*, MarkerCollectionConstPtr> item2Markers;
	VisualizerWidget* currentVisualizer;
	//QWidget* pane;
    QMainWindow* pane;
	QTreeWidget* treeWidget;
};

#endif // TOOLBOXMAIN_H
