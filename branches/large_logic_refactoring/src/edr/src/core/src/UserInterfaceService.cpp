#include "CorePCH.h"
#include "UserInterfaceService.h"
#include <iostream>

//--------------------------------------------------------------------------------------------------
UserInterfaceService::UserInterfaceService()
	: mainWindow(nullptr)
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

core::Window* UserInterfaceService::getMainWindow()
{
    return mainWindow;
}

void UserInterfaceService::setMainWindow( core::Window* mainWindow )
{
    this->mainWindow = mainWindow;
}
