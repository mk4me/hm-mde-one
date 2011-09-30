#ifndef TOOLBOXMAIN_H
#define TOOLBOXMAIN_H

#include <utils/Debug.h>
#include <osgViewer/CompositeViewer>
#include <QtCore/QVector>

#include <core/PluginLoader.h>
#include <core/Window.h>
#include <core/DataManager.h>

#include "ui_toolboxmaindeffile.h"
#include <core/Filesystem.h>
#include "MainWindow.h"
class UserInterfaceService;
class ServiceManager;
class SceneGraphWidget;
class EDRConsoleWidget;
class VisualizerManager;
class DataProcessorManager;
class VisualizerWidget;


class ToolboxMain : public core::MainWindow, private Ui::EDRMain
{
public:
	Q_OBJECT
public:
	ToolboxMain() : MainWindow() {}

public:
	virtual void init( core::PluginLoader* pluginLoader );

public slots:
	
	void onOpen();

	void onExit();
	void onCustomAction();
	void onCustomAction(bool triggered);

	void onSaveLayout();
	void onOpenLayout();
	void onShowSavedLayouts();

	void addLayoutsToMenu( const core::Filesystem::Path &dir );
	void onDockWidgetVisiblityChanged(bool visible);
	void onLayoutTriggered();

private slots:
	//! Wype³nia podmenu akcjami dla dostêpnych okien.
	//! \param target Menu do wype³nienia.
	void populateWindowMenu();
	//! Wype³nia podmenu akcjami tworz¹cymi wizualizatory.
	void populateVisualizersMenu();
	//!
	void actionCreateVisualizer();

	void createWorkflow();

private:
	//!
	void populateWindowMenu( QMenu* menu );
	//!
	void populateVisualizersMenu( QMenu* menu );

	void initializeUI();

	//! Natywne dodanie opcji do menu.
	virtual void onAddMenuItem( const std::string& path, bool checkable, bool initialState );
	//! Natywne usuniêcie opcji z menu.
	virtual void onRemoveMenuItem( const std::string& path );
	
	void openLayout( const QString& path );
};

#endif // TOOLBOXMAIN_H
