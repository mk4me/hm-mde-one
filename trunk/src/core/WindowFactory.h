#ifndef WINDOWS_FACTORY_H
#define WINDOWS_FACTORY_H

#include <core/MainWindow.h>
#include "QTMainWindow.h"
#include <string>

class MainWindowFactory
{
public:
    static IMainWindow* CreateMainWindow(std::string libraryName, void *object);
protected:
private:
};

#endif //WINDOWS_FACTORY_H