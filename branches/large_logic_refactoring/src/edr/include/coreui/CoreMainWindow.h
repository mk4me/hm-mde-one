/********************************************************************
	created:	2011/08/01
	created:	1:8:2011   8:37
	filename: 	CoreMainWindow.h
	author:		Wojciech Kniec

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_CORE__MAINWINDOW_H__
#define HEADER_GUARD_CORE__MAINWINDOW_H__

#include <coreui/Export.h>
#include <corelib/SmartPtr.h>
#include <corelib/IPlugin.h>
#include <QtCore/QSettings>
#include <QtGui/QMainWindow>
#include <corelib/Filesystem.h>
#include <boost/function.hpp>

class QSplashScreen;

namespace core {

	class IApplication;

};

namespace coreUI {

	class CoreConsoleWidget;

    template<class T>
    T* createNamedObject(const QString & objectName) {
        UTILS_STATIC_ASSERT((boost::is_base_of<QObject, T>::value), "Object must be derrived from QtCore::QObject");
        UTILS_ASSERT(objectName.isEmpty() == false);
        T* ret = new T();
        ret->setObjectName(objectName);
        return ret;
    }

	class COREUI_EXPORT CoreMainWindow : public QMainWindow
	{
		Q_OBJECT

	public:

		typedef boost::function<void()> CloseUpOperations;

	private:

		//! Widget ze sceną jakiegoś grafu OSG.
		//SceneGraphWidget* widgetSceneGraph;
		//! Widget konsoli.
		CoreConsoleWidget* widgetConsole;
        //! Lista zasobów.
        std::vector<std::string> resourcesPaths;
        //! Lista skórek dla UI
        std::vector<core::Filesystem::Path> applicationSkinsPaths;
		//! SplashScreenAplikacji
        QSplashScreen * splashScreen_;
		//TODO
		//trzeba zobaczyć czy nie wystarczą sygnały i sloty
		//! operacje na zamknięcie aplikacji
		CloseUpOperations closeUpOperations_;

	private:

		//! \return Customizowany splash screen dla danego widoku aplikacji, jeśli null to domyslny splash screen
		virtual QSplashScreen * createSplashScreen();
		virtual void customViewInit(QWidget * console) = 0;

	protected:

		CoreMainWindow(const CloseUpOperations & closeUpOperations);

	public:

		virtual ~CoreMainWindow();

		//! Dostarcza splash screen na potrzeby inicjalizacji aplikacji
		QSplashScreen * splashScreen();
		//! Inicjalizuje aplikację
		void init(core::IApplication * coreApplication);

		//! \param path Ścieżka stylu
		void setStyle(const core::Filesystem::Path& path);
		//! \param path Ścieżka stylu
		//! \return Czy udało się załadowac styl
		bool trySetStyle(const core::Filesystem::Path& path);
		//! \param styleName Nazwa stylu
		void setStyleByName(const std::string& styleName);
		//! \param styleName Nazwa stylu
		//! \return Czy udało się załadować styl o podanej nazwie
		bool trySetStyleByName(const std::string& styleName);

		//! \return Widget konsoli, który przechodzi pod kontrolę obiektu dziedziczącego - musi jakoś obsłużyć konsolę a na koniec ją usunąć
		CoreConsoleWidget* getConsole();

		virtual void showSplashScreenMessage(const QString & message);

        const core::Filesystem::Path & getApplicationSkinsFilePath(int i);

        int getApplicationSkinsFilePathCount();
		// QWidget
	protected:
		//!
		virtual void closeEvent(QCloseEvent* event);

	protected:
		//! Odczytuje ustawienia aplikacji.
		//! \param settings
		//! \param readGeometry
		virtual void readSettings(const QSettings& settings, bool readGeometry);
		//! Zapisuje ustawienia aplikacji.
		virtual void writeSettings();
	};
}

#endif
