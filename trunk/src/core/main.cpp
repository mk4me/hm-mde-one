#include "CorePCH.h"
#include <QtGui/QApplication>
#include "ToolboxMain.h"
#include <core/PluginLoader.h>
#include <utils/LeakDetection.h>

int main(int argc, char *argv[])
{
    int result = 0;
    {
        QApplication application(argc, argv);
        ToolboxMain window;
        window.show();
        result = application.exec();
    }

    // zwolnienie bibliotek
    core::PluginLoader::freeLibraries();
    return result;
}
