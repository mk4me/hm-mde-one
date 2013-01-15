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
#include <core/Window.h>
#include <core/Filesystem.h>
#include "ActionsGroupManager.h"

namespace core {

	class PluginLoader;

	class ExtendedCustomApplication;

	class UserInterfaceService;
	class ServiceManager;
	class EDRConsoleWidget;
	class VisualizerManager;
	class DataProcessorManager;
	class VisualizerWidget;

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

		//! Widget ze sceną jakiegoś grafu OSG.
		//SceneGraphWidget* widgetSceneGraph;
		//! Widget konsoli.
		EDRConsoleWidget* widgetConsole;
		//! Czy update jest włączony?
		bool updateEnabled;
		//! Timer wyznaczający update'y wizualizatorów.
		QTimer visualizerTimer;
        //! Timer wyznaczający updaty dla serwisów
		QTimer serviceTimer;
		//! Pluginy.
		core::PluginLoader* pluginLoader;
        //! Dostęp do managerów aplikacji.
        ExtendedCustomApplication * coreApplication;

        //! Lista zasobów.
        std::vector<std::string> resourcesPaths;

        //! Lista skórek dla UI
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

		virtual void init( core::PluginLoader* pluginLoader, ExtendedCustomApplication * coreApplication );

		static MainWindow * getInstance()
        {
            return instance;
        }
	public slots:
		//! Aktualizacja wizualizatorów.
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

		//! Rejestruje wbudowane usługi.
		void registerCoreServices();
		//! Rejestruje wbudowane źródła danych.
		void registerCoreDataSources();
		//! Rejestruje wbudowane typy domenowe
		void registerCoreDomainTypes();

        //! Szuka na dysku zasobów.
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

		void unpackPlugin(const core::PluginPtr & plugin);

		void safeRegisterPlugin(const core::PluginPtr & plugin);
		void safeRegisterService(const plugin::IServicePtr & service);
		void safeRegisterSource(const plugin::ISourcePtr & source);
		void safeRegisterParser(const plugin::IParserPtr & parser);
		void safeRegisterObjectWrapperPrototype(const core::ObjectWrapperConstPtr & prototype);
		void safeRegisterVisualizer(const plugin::IVisualizerPtr & visualizer);
		void safeRegisterDataProcessor(const plugin::IDataProcessorPtr & dataProcessor);
		void safeRegisterDataSource(const plugin::IDataSourcePtr & dataSource);

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
