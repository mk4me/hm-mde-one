#include "CoreUiPCH.h"
#include <coreui/CoreMainWindow.h>
#include <corelib/PluginCommon.h>
#include <QtWidgets/QSplashScreen>
#include <coreui/CoreConsoleWidget.h>
#include <coreui/CoreDockWidget.h>
#include <QtCore/QFile>
#include <QtCore/QSettings>
#include <QtWidgets/QMessageBox>
#include <QtGui/QCloseEvent>
#include <QtCore/QCoreApplication>

PLUGIN_DEFINE_CORE_APPLICATION_ACCESSOR

//WA�NE!!
//tak inicjalizujemy resourcy wkompilowane w biblioteki statyczne linkowane do aplikacji - w naszym przypadku to Core jest tak� bibliotek� i jego resourcy musza by� jawnie inicjalizowane
//Nazwa resourc�w musi by� unikalna poniewa� Qt "miesza" nazwy metod z nazwamy plik�w resourc�w kt�re chcemy inicjalizowa� tworz�c unikalne statyczne funkcje na potrzeby inicjalizacji
//link: http://developer.qt.nokia.com/doc/qt-4.8/resources.html - sam d� stronki
inline void initCoreResources() { Q_INIT_RESOURCE(CoreIcons); }

using namespace coreUI;

void CoreMainWindow::setStyle( const core::Filesystem::Path& path )
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


bool CoreMainWindow::trySetStyle( const core::Filesystem::Path& path )
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
            //setStyle(path);
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
	widgetConsole(new CoreConsoleWidget()), closeUpOperations_(closeUpOperations), closeConfirmationRequired_(true),
	closePerformed(false)
{
	initCoreResources();

	connect(&consoleTimer, SIGNAL(timeout()), widgetConsole, SLOT(flushQueue()));

	consoleTimer.start(1000/25);
}

QSplashScreen * CoreMainWindow::splashScreen()
{
	if(splashScreen_ == nullptr){
    
		splashScreen_ = new QSplashScreen(this);

		initializeSplashScreen(splashScreen_);

		splashScreen_->finish(this);
		splashScreen_->show();
	}	

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

	//szukaj styli qt
	auto temp = core::Filesystem::listFiles(plugin::getResourcesPath() / "app_skins", true, ".qss");
	applicationSkinsPaths.insert(applicationSkinsPaths.end(), temp.begin(), temp.end());

    readSettings(QSettings(), true);
	customViewInit(widgetConsole);
}

CoreMainWindow::~CoreMainWindow()
{
	if(splashScreen_ != nullptr){
		delete splashScreen_;
	}
}

const core::Filesystem::Path & CoreMainWindow::getApplicationSkinsFilePath(int i)
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

void CoreMainWindow::setCloseConfirmationRequired(const bool required)
{
	closeConfirmationRequired_ = required;
}

const bool CoreMainWindow::closeConfirmationRequired() const
{
	return closeConfirmationRequired_;
}

void CoreMainWindow::onClose()
{
	if(closePerformed == false){

		consoleTimer.stop();

		closePerformed = true;

		writeSettings();

		if(closeUpOperations_.empty() == false){
			try{
				closeUpOperations_();
			}catch(...){

			}
		}
	}
}

void CoreMainWindow::closeEvent(QCloseEvent* event)
{
	bool close = true;

	if(closeConfirmationRequired_ == true){

		if(QMessageBox::question( this, tr("Confirm exit"), tr("Are You sure You want to exit application?"),
			QMessageBox::Yes | QMessageBox::No, QMessageBox::No ) == QMessageBox::No){

				close = false;
		}

	}

	if(close == true){

		onClose();

		event->accept();
	} else {
		event->ignore();
	}
}