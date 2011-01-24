#include "CorePCH.h"
#include <QtGui/QApplication>
#include "ToolboxMain.h"
#include <core/PluginLoader.h>
#include <utils/LeakDetection.h>

#include <core/Log.h>

// TODO: najlepiej by�oby te ifdefy zebra� tak, �eby wyst�powa�y w tylko jednym miejscu.
#ifdef CORELIB_ENABLE_LOGGING
#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include "OsgNotifyHandlerLog4cxx.h"
#endif 

using namespace core;

int main(int argc, char *argv[])
{
#ifdef CORELIB_ENABLE_LOGGING
    // za�adowanie paramet�w logowania
    log4cxx::PropertyConfigurator::configure("B:\\edytor_trunk\\bin\\Debug\\data\\resources\\settings\\log.ini");
    osg::setNotifyHandler( new OsgNotifyHandlerLog4cxx(log4cxx::Logger::getLogger( "osg" ) ));
#endif

    osg::ArgumentParser arguments(&argc,argv);
    arguments.getApplicationUsage()->setApplicationName(arguments.getApplicationName());
    arguments.getApplicationUsage()->setDescription(arguments.getApplicationName()+" example usage of EDR.");
    arguments.getApplicationUsage()->setCommandLineUsage(arguments.getApplicationName()+" [options] filename");
    arguments.getApplicationUsage()->addCommandLineOption("-h or --help","Display this information");

    // czy wy�wietlamy pomoc?
    if (arguments.read("-h") || arguments.read("--help")) 
    {
        arguments.getApplicationUsage()->write(std::cout);
        return 1;
    }

    // nazwa pliku do za�adowania
    std::string filePath;
    for (int i=1; i<arguments.argc() && filePath.empty(); ++i) 
    {
        if (arguments.isString(i)) 
        {
            filePath = arguments[i];
        }
    }

    int result = 0;
    {
        QApplication application(argc, argv);
        ToolboxMain window;
        window.show();
        if (!filePath.empty()) 
        {
            window.openFile(filePath);
        }
        result = application.exec();
    }

    // zwolnienie bibliotek
    core::PluginLoader::freeLibraries();

#ifdef CORELIB_ENABLE_LOGGING
    // trzeba przywr�ci� domy�lnego notifiera, inaczej b�d� b��dy z destruktorem
    // (konflikty zmiennych globalnych)
    osg::setNotifyHandler( new osg::StandardNotifyHandler() );
#endif
    return result;
}
