#include "CorePCH.h"
#include "MainWindow.h"
#include <core/PluginCommon.h>
#include <QtGui/QSplashScreen>
#include "CoreConsoleWidget.h"
#include "CoreDockWidget.h"
#include <QtCore/QFile>
#include <QtCore/QSettings>

PLUGIN_DEFINE_CORE_APPLICATION_ACCESSOR

using namespace coreUI;
using namespace core;

void MainWindow::setStyle( const Filesystem::Path& path )
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


bool MainWindow::trySetStyle( const Filesystem::Path& path )
{
    try {
        setStyle(path);
    } catch(...) {
        return false;
    }

    return true;
}


void MainWindow::setStyleByName( const std::string& styleName )
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

bool MainWindow::trySetStyleByName( const std::string& styleName )
{
    try {
        setStyleByName(styleName);
    } catch(...) {
        return false;
    }

    return true;
}

MainWindow::MainWindow(): QMainWindow(nullptr), splashScreen_(nullptr),	widgetConsole(new CoreConsoleWidget())
{
	//TODO
	//szukaj styli qt
	//temp = Filesystem::listFiles(resourcesPath, true, ".qss");
	//applicationSkinsPaths.insert(applicationSkinsPaths.end(), temp.begin(), temp.end());
}

QSplashScreen * MainWindow::splashScreen()
{
	if(splashScreen_ == nullptr){
    
		splashScreen_ = createSplashScreen();

		if(splashScreen_ == nullptr){
			splashScreen_ = new QSplashScreen();
		}

		splashScreen_->show();
	}

	splashScreen_->finish(this);

	return splashScreen_;
}

void MainWindow::showSplashScreenMessage(const QString & message)
{
	splashScreen()->showMessage(message);
}

CoreConsoleWidget* MainWindow::getConsole()
{
	return widgetConsole;
}

void MainWindow::init(core::IApplication * coreApplication)
{
	plugin::__coreApplication = coreApplication;
    readSettings(QSettings(), true);
	customViewInit(widgetConsole);
}

MainWindow::~MainWindow()
{
	if(splashScreen_ != nullptr){
		delete splashScreen_;
	}
}

const Filesystem::Path & MainWindow::getApplicationSkinsFilePath(int i)
{
	return applicationSkinsPaths[i];
}

int MainWindow::getApplicationSkinsFilePathCount()
{
	return applicationSkinsPaths.size();
}

void MainWindow::readSettings( const QSettings& settings, bool readGeometry )
{
    if ( settings.contains("WindowState") ) {
        restoreState(settings.value("WindowState").toByteArray());
    }
    if ( readGeometry && settings.contains("Geometry") ) {
        restoreGeometry(settings.value("Geometry").toByteArray());
    }
}

void MainWindow::writeSettings()
{
    QSettings settings;
    settings.setValue("Geometry", saveGeometry());
    settings.setValue("WindowState", saveState());
}

void MainWindow::setCloseUpOperations(const CloseUpOperations & closeUp)
{
	closeUpOperations_ = closeUp;
}

void MainWindow::closeEvent(QCloseEvent* event)
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
