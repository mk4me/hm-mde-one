#include "CorePCH.h"
#include "MainWindow.h"
#include "ApplicationCommon.h"
#include <QtGui/QSplashScreen>
#include "EDRConsoleWidget.h"
#include "EDRDockWidget.h"
#include <QtCore/QFile>
#include <QtCore/QSettings>


using namespace core;

void MainWindow::setStyle( const core::Filesystem::Path& path )
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


bool MainWindow::trySetStyle( const core::Filesystem::Path& path )
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

void MainWindow::saveScreen(const QPixmap & pixmap)
{
	try{
		pixmap.save(QString((screenshotsPath_ / "screen_%1.png").string().c_str()).arg(QDateTime::currentDateTime().toString()));
	}catch(...){

	}
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


//Q_DECLARE_METATYPE ( Filesystem::Path );

MainWindow::MainWindow(): QMainWindow(nullptr), splashScreen_(nullptr), screenshotsPath_(getPathInterface()->getUserDataPath() / "screenshots"),
	widgetConsole(new EDRConsoleWidget())
{
    if(Filesystem::pathExists(screenshotsPath_) == false){
		Filesystem::createDirectory(screenshotsPath_);
	}
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

EDRConsoleWidget* MainWindow::getConsole()
{
	return widgetConsole;
}

void MainWindow::init()
{
    readSettings(QSettings(), true);
	customViewInit(widgetConsole);
}

MainWindow::~MainWindow()
{
	if(splashScreen_ != nullptr){
		delete splashScreen_;
	}
}

void MainWindow::findResources(const Filesystem::Path & resourcesPath)
{
    // TODO : potrzebna rewizja tego kodu...
    resourcesPaths.clear();
    //szukaj shaderów
    std::vector<std::string> ext;
    ext.push_back(".frag");
    ext.push_back(".vert");
    try {
        std::vector<std::string> temp = Filesystem::listFiles(resourcesPath, true, ext);
        resourcesPaths.insert(resourcesPaths.end(), temp.begin(), temp.end());
        //szukaj mesh
        temp = Filesystem::listFiles(resourcesPath, true, ".fmesh");
        resourcesPaths.insert(resourcesPaths.end(), temp.begin(), temp.end());
        //szukaj styli qt
        temp = Filesystem::listFiles(resourcesPath, true, ".qss");
        applicationSkinsPaths.insert(applicationSkinsPaths.end(), temp.begin(), temp.end());
    } catch(std::exception& e) {
        CORE_LOG_INFO("Finding resources exception: " << e.what());
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

void MainWindow::closeEvent(QCloseEvent* event)
{
    writeSettings();
    QMainWindow::closeEvent(event);
}

QDockWidget* MainWindow::embeddWidget( QWidget* widget, const ActionsGroupManager & widgetActions, const QString& name, const QString& style, const QString& sufix,
    Qt::DockWidgetArea area /*= Qt::AllDockWidgetAreas*/)
{
    // dodajemy widget dokowalny
    EDRDockWidget* dock = new EDRDockWidget( name, this, Qt::WindowTitleHint);
    dock->setAllowedAreas(area);
    dock->setObjectName(name + widget->objectName() + "WIDGET" + sufix);
    dock->setStyleSheet(style);
    dock->setWidget(widget);
    dock->setPermanent(true);
    QObject::connect( dock, SIGNAL(visibilityChanged(bool)), this, SLOT(onDockWidgetVisiblityChanged(bool)) );

	//TODO

	/*EDRTitleBar * titleBar = supplyWithEDRTitleBar(dock);

	for(auto groupIT = widgetActions.begin(); groupIT != widgetActions.end(); ++groupIT){
	std::map<int, QObject *> allObjects;
	(*groupIT).getAllObjects(allObjects);
	for(auto objectIT = allObjects.begin(); objectIT != allObjects.end(); ++objectIT){
	titleBar->addObject(objectIT->second, IEDRTitleBar::Left);
	}
	}*/

    return dock;
}
