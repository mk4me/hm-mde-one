#include "CorePCH.h"
#include <core/Log.h>
#include "UserInterfaceService.h"
#include <iostream>

#include <core/IModel.h>
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



AsyncResult UserInterfaceService::init( IServiceManager* serviceManager, osg::Node* sceneRoot )
{
    return AsyncResult_Complete;
}

AsyncResult UserInterfaceService::loadData( IServiceManager* serviceManager, IDataManager* dataManager )
{
    return AsyncResult_Complete;
}

AsyncResult UserInterfaceService::update( double time, double timeDelta )
{
    return AsyncResult_Complete;
}

AsyncResult UserInterfaceService::lateUpdate( double time, double timeDelta )
{
    return AsyncResult_Complete;
}

AsyncResult UserInterfaceService::compute()
{
    return AsyncResult_Complete;
}

IWidget* UserInterfaceService::getWidget()
{
    return NULL;
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