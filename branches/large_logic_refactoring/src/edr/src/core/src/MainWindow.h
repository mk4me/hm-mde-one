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
#include <QtCore/QObject>
#include <QtCore/QSettings>
#include <QtGui/QMainWindow>
#include <core/Window.h>
#include <core/Filesystem.h>

class QSplashScreen;

namespace core {

	class EDRConsoleWidget;
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

	private:

		//! Widget ze sceną jakiegoś grafu OSG.
		//SceneGraphWidget* widgetSceneGraph;
		//! Widget konsoli.
		EDRConsoleWidget* widgetConsole;
        //! Lista zasobów.
        std::vector<std::string> resourcesPaths;
        //! Lista skórek dla UI
        std::vector<Filesystem::Path> applicationSkinsPaths;
		//! SplashScreenAplikacji
        QSplashScreen * splashScreen_;
		//! Ścieżka do screenshotów z aplikacji
		Filesystem::Path screenshotsPath_;

	private:

		//! \return Customizowany splash screen dla danego widoku aplikacji, jeśli null to domyslny splash screen
		virtual QSplashScreen * createSplashScreen() = 0;
		virtual void customViewInit(QWidget * console) = 0;

	public:

		MainWindow();
		virtual ~MainWindow();

		//! Dostarcza splash screen na potrzeby inicjalizacji aplikacji
		QSplashScreen * splashScreen();
		//! Inicjalizuje aplikację
		void init();

		//! \param path Ścieżka stylu
		void setStyle(const Filesystem::Path& path);
		//! \param path Ścieżka stylu
		//! \return Czy udało się załadowac styl
		bool trySetStyle(const Filesystem::Path& path);
		//! \param styleName Nazwa stylu
		void setStyleByName(const std::string& styleName);
		//! \param styleName Nazwa stylu
		//! \return Czy udało się załadować styl o podanej nazwie
		bool trySetStyleByName(const std::string& styleName);

		//! Metoda obsługująca akcje wizualizatorów
		//! \param visWidget Widget wizualizatora
		virtual void setCurrentVisualizerActions(VisualizerWidget * visWidget) = 0;

		//! \return Widget konsoli, który przechodzi pod kontrolę obiektu dziedziczącego - musi jakoś obsłużyć konsolę a na koniec ją usunąć
		EDRConsoleWidget* getConsole();

		virtual void showSplashScreenMessage(const QString & message);

	public slots:
		//! \param pixmap Screenshot aplikacji do zapisu
		void saveScreen(const QPixmap & pixmap);

	private:

        //! Szuka na dysku zasobów.
        void findResources(const Filesystem::Path & resourcesPath);

        const Filesystem::Path & getApplicationSkinsFilePath(int i);

        int getApplicationSkinsFilePathCount();

		//! Opakowuje zadany widget QDockWidgetem.
		//! \param widget
		//! \param name
		//! \param style
		//! \param area
		QDockWidget* embeddWidget(QWidget* widget, const ActionsGroupManager& widgetActions, const QString& name, const QString& style, const QString& sufix, Qt::DockWidgetArea area = Qt::AllDockWidgetAreas);

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
}

#endif
