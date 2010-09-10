#ifndef USER_INTERFACE_SERVICE_H
#define USER_INTERFACE_SERVICE_H


#include <string>
#include <PluginsInterfaces.h>

#include <QtGui/QMainWindow>
#include <QtGui>
#include <QMenu>
#include <QDir>
#include <QAction>
#include <QVector>

#include "../include/GlobalServicesIDs.h"
#include "../include/SimpleFunctors.h"
#include "../include/UserInterface.h"


//--------------------------------------------------------------------------------------------------
// Plugin Service
//--------------------------------------------------------------------------------------------------
class UserInterfaceService: public IBaseService, public IUserInterface
{
public:
    UserInterfaceService();
    virtual ~UserInterfaceService();

    //IBaseService
    virtual AsyncResult OnTick(double delta); 
    virtual AsyncResult OnAdded(IServiceManager* serviceManager); 

    //IUserInterface
    virtual void AddMenu(std::string menuName);
    virtual void AddAction(std::string menuName, std::string actionName);
    virtual void AddAction(std::string menuName, std::string actionName, std::string slotMethodNameFromMainMenu);
    virtual void InicializeServices(std::string typeName, void *object);

    void* GetMainObject();

protected: 


private: 
    M_DECLARE_CLASS(); 

    IMainWindow* m_pMainWindow;
    void* m_pMainObject;

    ServiceManager* m_pServiceManager;
};


#endif //USER_INTERFACE_SERVICE_H