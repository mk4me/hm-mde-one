#include "CorePCH.h"
#include <QtGui/QApplication>
#include "ToolboxMain.h"
#include <core/PluginLoader.h>
#include <utils/LeakDetection.h>
#include <core/Log.h>


using namespace core;

int main(int argc, char *argv[])
{
    core::LogInitializer logger("data/resources/settings/log.ini");

    osg::ArgumentParser arguments(&argc,argv);
    arguments.getApplicationUsage()->setApplicationName(arguments.getApplicationName());
    arguments.getApplicationUsage()->setDescription(arguments.getApplicationName()+" example usage of EDR.");
    arguments.getApplicationUsage()->setCommandLineUsage(arguments.getApplicationName()+" [options] filename");
    arguments.getApplicationUsage()->addCommandLineOption("-h or --help","Display this information");

    // czy wyœwietlamy pomoc?
    if (arguments.read("-h") || arguments.read("--help")) 
    {
        arguments.getApplicationUsage()->write(std::cout);
        return 1;
    }

    // nazwa pliku do za³adowania
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
        logger.setConsoleWidget( window.getConsole() );
        window.show();
        if (!filePath.empty()) 
        {
            window.openFile(filePath);
        }
        result = application.exec();
        logger.setConsoleWidget(NULL);
    }

    // zwolnienie bibliotek
    core::PluginLoader::freeLibraries();
    return result;
}
