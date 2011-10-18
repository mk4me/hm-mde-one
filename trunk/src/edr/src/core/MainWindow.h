/********************************************************************
	created:	2011/08/01
	created:	1:8:2011   8:37
	filename: 	MainWindow.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__MAINWINDOW_H__
#define HEADER_GUARD_CORE__MAINWINDOW_H__

#include <utils/Debug.h>
#include <osgViewer/CompositeViewer>
#include <QtCore/QVector>

#include <core/PluginLoader.h>
#include <core/Window.h>
#include <core/DataManager.h>

#include <core/Filesystem.h>

class UserInterfaceService;
class ServiceManager;
class SceneGraphWidget;
class EDRConsoleWidget;
class VisualizerManager;
class DataProcessorManager;
class VisualizerWidget;

namespace core {

	class MainWindow : public QMainWindow, public core::Window
	{
		Q_OBJECT

	protected:    
		//! Widget ze scen¹ jakiegoœ grafu OSG.
		SceneGraphWidget* widgetSceneGraph;
		//! Widget konsoli.
		EDRConsoleWidget* widgetConsole; 
		//! Czy update jest w³¹czony?
		bool updateEnabled;
		//! Timer wyznaczaj¹cy update'y wizualizatorów.
		QTimer visualizerTimer;
        //! Timer wyznaczaj¹cy updaty dla serwisów
		QTimer serviceTimer;
		//! Korzeñ sceny.
		//osg::ref_ptr<osg::Node> sceneRoot;
		//! Pluginy.
		core::PluginLoader* pluginLoader;
		//! Timer u¿ywany gdy wyœwietlamy w trybie composite.
		QTimer viewerFrameTimer;

	protected:
		const QTimer& getVisualizerTimer() const { return visualizerTimer; }
		QTimer& getVisualizerTimer() { return visualizerTimer; }

	public:
		MainWindow();
		virtual ~MainWindow();

		virtual void init( core::PluginLoader* pluginLoader );

		
	public slots:
		//! Aktualizacja wizualizatorow.
		void updateVisualizers();
        void updateServices();

	public:
		void initializeConsole();
		void InitializeControlWidget();

	public:

		void loadData();

		EDRConsoleWidget* getConsole()
		{
			return widgetConsole;
		}

	public:
		void openFile( const std::string& path );

	protected:

		//! Rejestruje wbudowane us³ugi.
		void registerCoreServices();
		//! Rejestruje wbudowane Ÿród³a danych.
		void registerCoreDataSources();

		//! Rejestruje us³ugi pochodz¹ce z pluginów.
		void registerPluginsServices();
		//! Rejestruje parsery pochodz¹ce z pluginów.
		void registerPluginsParsers();
		//!
		void registerPluginsVisualizers();
		//!
		void registerPluginsDataProcessors();

		void registerPluginsDataSources();

		void registerPluginsWrapperFactories();


		//! Opakowuje zadany widget QDockWidgetem.
		//! \param widget
		//! \param name
		//! \param style
		//! \param area
		QDockWidget* embeddWidget(QWidget* widget, std::vector<QObject*>& widgetActions, const QString& name, const QString& style, const QString& sufix, Qt::DockWidgetArea area = Qt::AllDockWidgetAreas);

		void safeRegisterPlugin(const core::PluginPtr & plugin);
		void safeRegisterService(const core::IServicePtr & service);
		void safeRegisterParser(const core::IParserPtr & parser);
		void safeRegisterObjectFactory(const core::IObjectWrapperFactoryPtr & factory);
		void safeRegisterVisualizer(const core::IVisualizerPtr & visualizer);
		void safeRegisterDataProcessor(const core::IDataProcessorPtr & dataProcessor);
		void safeRegisterDataSource(const core::IDataSourcePtr & dataSource);

		// QWidget
	protected:
		//!
		virtual void closeEvent(QCloseEvent* event);

	protected: 
		//! Odczytuje ustawienia aplikacji.
		//! \param settings
		//! \param readGeometry
		void readSettings(const QSettings& settings, bool readGeometry);
		//! Zapisuje ustawienia aplikacji.
		void writeSettings();

	};

typedef boost::shared_ptr<MainWindow> MainWindowPtr;
typedef boost::shared_ptr<const MainWindow> MainWindowConstPtr;

}

#endif
