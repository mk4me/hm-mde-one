/********************************************************************
	created:	2011/08/01
	created:	1:8:2011   8:37
	filename: 	MainWindow.h
	author:		Wojciech Kniec

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_CORE__MAINWINDOW_H__
#define HEADER_GUARD_CORE__MAINWINDOW_H__

#include <core/SmartPtr.h>
#include <core/Plugin.h>

class QObject;
class QSplashScreen;

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtCore/QSettings>
#include <QtGui/QMainWindow>

//#include <utils/Debug.h>
namespace core {

	class PluginLoader;
	class IManagersAccessor;
}

#include <core/Window.h>
#include <core/Filesystem.h>

#include "ActionsGroupManager.h"

class UserInterfaceService;
class ServiceManager;
class EDRConsoleWidget;
class VisualizerManager;
class DataProcessorManager;
class VisualizerWidget;

namespace core {

    template<class T>
    T* createNamedObject(const QString & objectName) {
        UTILS_STATIC_ASSERT((boost::is_base_of<QObject, T>::value), "Object must be derrived from QtCore::QObject");
        UTILS_ASSERT(objectName.isEmpty() == false);
        T* ret = new T();
        ret->setObjectName(objectName);
        return ret;
    }

	class MainWindow : public QMainWindow, public core::Window
	{
		Q_OBJECT

	protected:

        static MainWindow * instance;

		//! Widget ze scen� jakiego� grafu OSG.
		//SceneGraphWidget* widgetSceneGraph;
		//! Widget konsoli.
		EDRConsoleWidget* widgetConsole;
		//! Czy update jest w��czony?
		bool updateEnabled;
		//! Timer wyznaczaj�cy update'y wizualizator�w.
		QTimer visualizerTimer;
        //! Timer wyznaczaj�cy updaty dla serwis�w
		QTimer serviceTimer;
		//! Pluginy.
		core::PluginLoader* pluginLoader;
        //! Dost�p do manager�w aplikacji.
        core::IManagersAccessor* managersAccessor;

        //! Lista zasob�w.
        std::vector<std::string> resourcesPaths;

        //! Lista sk�rek dla UI
        std::vector<core::Filesystem::Path> applicationSkinsPaths;

        QSplashScreen * splashScreen_;

	protected:
		const QTimer& getVisualizerTimer() const { return visualizerTimer; }
		QTimer& getVisualizerTimer() { return visualizerTimer; }

        QSplashScreen * splashScreen();

        void showSplashScreenMessage(const QString & message, int alignment = Qt::AlignLeft, const QColor & color = Qt::black);

	public:
		MainWindow();
		virtual ~MainWindow();

		virtual void init( core::PluginLoader* pluginLoader, core::IManagersAccessor * managersAccessor );

		static MainWindow * getInstance()
        {
            return instance;
        }
	public slots:
		//! Aktualizacja wizualizatorow.
		void updateVisualizers();
        void updateServices();

		void saveScreen(const QPixmap & pixmap);

	public:
		void initializeConsole();
		//void InitializeControlWidget();

        void setStyle(const core::Filesystem::Path& path);
        bool trySetStyle(const core::Filesystem::Path& path);
        void setStyleByName(const std::string& styleName);
        bool trySetStyleByName(const std::string& styleName);

        virtual void setCurrentVisualizerActions(VisualizerWidget * visWidget) = 0;


	public:

		void loadData();

		EDRConsoleWidget* getConsole()
		{
			return widgetConsole;
		}

	public:
		void openFile( const std::string& path );

	protected:

        template<class T>
        void registerCustomWrapperFactory(const T * dummy = nullptr)
        {
            safeRegisterObjectFactory( IObjectWrapperFactoryPtr(new ObjectWrapperFactory<T>()) );
        }

		//! Rejestruje wbudowane us�ugi.
		void registerCoreServices();
		//! Rejestruje wbudowane �r�d�a danych.
		void registerCoreDataSources();

		//! Rejestruje us�ugi pochodz�ce z plugin�w.
		void registerPluginsServices();
		//! Rejestruje �r�d�a pochodz�ce z plugin�w.
		void registerPluginsSources();
		//! Rejestruje parsery pochodz�ce z plugin�w.
		void registerPluginsParsers();
		//!
		void registerPluginsVisualizers();
		//!
		void registerPluginsDataProcessors();

		void registerPluginsDataSources();

		void registerPluginsWrapperFactories();

        //! Szuka na dysku zasob�w.
        void findResources(const std::string& resourcesPath);

        const core::Filesystem::Path& getApplicationSkinsFilePath(int i)
        {
            return applicationSkinsPaths[i];
        }

        int getApplicationSkinsFilePathCount()
        {
            return applicationSkinsPaths.size();
        }

		//! Opakowuje zadany widget QDockWidgetem.
		//! \param widget
		//! \param name
		//! \param style
		//! \param area
		QDockWidget* embeddWidget(QWidget* widget, const ActionsGroupManager& widgetActions, const QString& name, const QString& style, const QString& sufix, Qt::DockWidgetArea area = Qt::AllDockWidgetAreas);

		void safeRegisterPlugin(const core::PluginPtr & plugin);
		void safeRegisterService(const core::IServicePtr & service);
		void safeRegisterSource(const core::ISourcePtr & source);
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

typedef core::shared_ptr<MainWindow> MainWindowPtr;
typedef core::shared_ptr<const MainWindow> MainWindowConstPtr;

}

#endif
