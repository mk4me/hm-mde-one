#ifndef USER_INTERFACE_SERVICE_H
#define USER_INTERFACE_SERVICE_H


#include <string>
#include <core/PluginsInterfaces.h>

#include <QtGui/QMainWindow>
#include <QtGui/QtGui>
#include <QtGui/QMenu>
#include <QtCore/QDir>
#include <QtGui/QAction>
#include <QtCore/QVector>

#include <core/GlobalServicesIDs.h>
#include <core/SimpleFunctors.h>
#include <core/UserInterface.h>

class IModel;
class IDataManager;

//--------------------------------------------------------------------------------------------------
// Plugin Service
//--------------------------------------------------------------------------------------------------
class UserInterfaceService: public IService, public IUserInterface
{
    UNIQUE_ID('UI','SRVC');
public:
    UserInterfaceService();
    virtual ~UserInterfaceService();

    //IService
    virtual AsyncResult OnAdded(IServiceManager* serviceManager); 

    virtual void SetModel(IDataManager* dataManager);

    //IUserInterface
    virtual void AddMenu(std::string menuName);
    virtual void AddAction(std::string menuName, std::string actionName);
    virtual void AddAction(std::string menuName, std::string actionName, std::string slotMethodNameFromMainMenu);
    virtual void InicializeServices(std::string typeName, void *object);

    void* GetMainObject();

protected: 

    IMainWindow* m_pMainWindow;
    void* m_pMainObject;

    ServiceManager* m_pServiceManager;
};


#endif //USER_INTERFACE_SERVICE_H