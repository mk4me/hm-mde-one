#include "CorePCH.h"
#include "WindowFactory.h"


//--------------------------------------------------------------------------------------------------
IMainWindow* MainWindowFactory::CreateMainWindow( std::string libraryName, void *object )
{
    if(libraryName == "QT") {
        return (IMainWindow*)(new QTMainWindow(object));
    }

    return NULL;
}