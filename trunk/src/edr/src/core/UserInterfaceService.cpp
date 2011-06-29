#include "CorePCH.h"
#include "UserInterfaceService.h"
#include <iostream>

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