#include "CorePCH.h"
#include "UserInterfaceService.h"
#include "WindowFactory.h"
#include <iostream>

#include <core/IModel.h>
#include <core/IDataManager.h>

using namespace std;
M_DECLARED_CLASS(UserInterfaceService, kCLASSID_UserInterfaceService);


//--------------------------------------------------------------------------------------------------
UserInterfaceService::UserInterfaceService()
{

}

//--------------------------------------------------------------------------------------------------
UserInterfaceService::~UserInterfaceService()
{

}

//--------------------------------------------------------------------------------------------------
AsyncResult UserInterfaceService::OnAdded(IServiceManager* serviceManager)
{
    m_pServiceManager = (ServiceManager*)serviceManager;

    std::cout << "UserInterfaceService ADDED-test!" << std::endl; 
    return AsyncResult_Complete; 
}

//--------------------------------------------------------------------------------------------------
void UserInterfaceService::AddMenu(std::string menuName)
{
    m_pMainWindow->AddMenu(menuName);
}

//--------------------------------------------------------------------------------------------------
void UserInterfaceService::AddAction(std::string menuName, std::string actionName)
{
    m_pMainWindow->AddAction(menuName, actionName);
}

//--------------------------------------------------------------------------------------------------
void UserInterfaceService::AddAction( std::string menuName, std::string actionName, std::string slotMethodNameFromMainMenu )
{
    m_pMainWindow->AddAction(menuName, actionName, slotMethodNameFromMainMenu);
}
//--------------------------------------------------------------------------------------------------
void UserInterfaceService::InicializeServices( std::string typeName, void *object )
{
    m_pMainObject = object;
    m_pMainWindow = MainWindowFactory::CreateMainWindow(typeName, object);
}

//--------------------------------------------------------------------------------------------------
void* UserInterfaceService::GetMainObject()
{
    return m_pMainObject;
}

//--------------------------------------------------------------------------------------------------
void UserInterfaceService::SetModel(IDataManager* dataManager )
{

}
