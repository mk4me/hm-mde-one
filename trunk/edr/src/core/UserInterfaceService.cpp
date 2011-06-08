#include "CorePCH.h"
#include <core/Log.h>
#include "UserInterfaceService.h"
#include <iostream>

//#include <core/IModel.h>
#include <core/IDataManager.h>


//--------------------------------------------------------------------------------------------------
UserInterfaceService::UserInterfaceService()
:   name("UserInterface")
{
}

//--------------------------------------------------------------------------------------------------
UserInterfaceService::~UserInterfaceService()
{
}

// void UserInterfaceService::InicializeServices( std::string typeName, void *object )
// {
//     m_pMainObject = object;
//     m_pMainWindow = MainWindowFactory::CreateMainWindow(typeName, object);
// }

QWidget* UserInterfaceService::getWidget(std::vector<QObject*>& actions)
{
    return nullptr;
}

const std::string& UserInterfaceService::getName() const
{
    return name;
}

core::Window* UserInterfaceService::getMainWindow()
{
    return mainWindow;
}

void UserInterfaceService::setMainWindow( core::Window* mainWindow )
{
    this->mainWindow = mainWindow;
}