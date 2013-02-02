#include "CoreUiPCH.h"
#include <coreui/CoreMainWindow.h>
#include <corelib/PluginCommon.h>
#include <QtGui/QSplashScreen>
#include <coreui/CoreConsoleWidget.h>
#include <coreui/CoreDockWidget.h>
#include <QtCore/QFile>
#include <QtCore/QSettings>
#include <QtGui/QMessageBox>
#include <QtGui/QCloseEvent>

PLUGIN_DEFINE_CORE_APPLICATION_ACCESSOR

//WA¯NE!!
//tak inicjalizujemy resourcy wkompilowane w biblioteki statyczne linkowane do aplikacji - w naszym przypadku to Core jest tak¹ bibliotek¹ i jego resourcy musza byæ jawnie inicjalizowane
//Nazwa resourców musi byæ unikalna poniewa¿ Qt "miesza" nazwy metod z nazwamy plików resourców które chcemy inicjalizowaæ tworz¹c unikalne statyczne funkcje na potrzeby inicjalizacji
//link: http://developer.qt.nokia.com/doc/qt-4.8/resources.html - sam dó³ stronki
inline void initCoreResources() { Q_INIT_RESOURCE(CoreIcons); }

using namespace coreUI;
using namespace core;

void CoreMainWindow::setStyle( const Filesystem::Path& path )
{
    QFile file(QString::fromUtf8(path.string().c_str()));
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString style;
        style = file.readAll();
        file.close();
        setStyleSheet(style);
    } else {
        throw std::runtime_error(std::string("Problem loading file : ") + path.string() );
    }
}


bool CoreMainWindow::trySetStyle( const Filesystem::Path& path )
{
    try {
        setStyle(path);
    } catch(...) {
        return false;
    }

    return true;
}


void CoreMainWindow::setStyleByName( const std::string& styleName )
{
    int count = getApplicationSkinsFilePathCount();
    for (int i = 0; i < count; ++i) {
        const core::Filesystem::Path& path = getApplicationSkinsFilePath(i);
        if (path.stem() == styleName) {
            setStyle(path);
            return;
        }
    }

    throw std::runtime_error(std::string("Unable to set style : ") + styleName);
}

bool CoreMainWindow::trySetStyleByName( const std::string& styleName )
{
    try {
        setStyleByName(styleName);
    } catch(...) {
        return false;
    }

    return true;
}

CoreMainWindow::CoreMainWindow(const CloseUpOperations & closeUpOperations): QMainWindow(nullptr), splashScreen_(nullptr),
	widgetConsole(new CoreConsoleWidget()), closeUpOperations_(closeUpOperations)
{
	initCoreResources();
	//TODO
	//szukaj styli qt
	//temp = Filesystem::listFiles(resourcesPath, true, ".qss");
	//applicationSkinsPaths.insert(applicationSkinsPaths.end(), temp.begin(), temp.end());
}

QSplashScreen * CoreMainWindow::createSplashScreen()
{
	return new QSplashScreen();
}

QSplashScreen * CoreMainWindow::splashScreen()
{
	if(splashScreen_ == nullptr){
    
		splashScreen_ = createSplashScreen();

		if(splashScreen_ == nullptr){
			splashScreen_ = CoreMainWindow::createSplashScreen();
		}

		splashScreen_->show();
	}

	splashScreen_->finish(this);

	return splashScreen_;
}

void CoreMainWindow::showSplashScreenMessage(const QString & message)
{
	splashScreen()->showMessage(message);
}

CoreConsoleWidget* CoreMainWindow::getConsole()
{
	return widgetConsole;
}

void CoreMainWindow::init(core::IApplication * coreApplication)
{
	plugin::__coreApplication = coreApplication;
    readSettings(QSettings(), true);
	customViewInit(widgetConsole);
}

CoreMainWindow::~CoreMainWindow()
{
	if(splashScreen_ != nullptr){
		delete splashScreen_;
	}
}

const Filesystem::Path & CoreMainWindow::getApplicationSkinsFilePath(int i)
{
	return applicationSkinsPaths[i];
}

int CoreMainWindow::getApplicationSkinsFilePathCount()
{
	return applicationSkinsPaths.size();
}

void CoreMainWindow::readSettings( const QSettings& settings, bool readGeometry )
{
    if ( settings.contains("WindowState") ) {
        restoreState(settings.value("WindowState").toByteArray());
    }
    if ( readGeometry && settings.contains("Geometry") ) {
        restoreGeometry(settings.value("Geometry").toByteArray());
    }
}

void CoreMainWindow::writeSettings()
{
    QSettings settings;
    settings.setValue("Geometry", saveGeometry());
    settings.setValue("WindowState", saveState());
}

void CoreMainWindow::closeEvent(QCloseEvent* event)
{
	if(QMessageBox::question( nullptr, tr("Confirm exit"), tr("Are You sure You want to exit application?"),
		QMessageBox::Yes | QMessageBox::Abort, QMessageBox::Abort ) == QMessageBox::Yes){
	
		writeSettings();

		if(closeUpOperations_.empty() == false){
			try{
				closeUpOperations_();
			}catch(...){

			}
		}

		event->accept();
	} else {
		event->ignore();
	}
}
