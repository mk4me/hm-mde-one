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
    virtual AsyncResult init(IServiceManager* serviceManager, osg::Node* sceneRoot); 

    virtual AsyncResult loadData(IServiceManager* serviceManager, IDataManager* dataManager);

    //IUserInterface
    virtual void AddMenu(std::string menuName);
    virtual void AddAction(std::string menuName, std::string actionName);
    virtual void AddAction(std::string menuName, std::string actionName, std::string slotMethodNameFromMainMenu);
    virtual void InicializeServices(std::string typeName, void *object);
    
    virtual const std::string& getName() const
    {
        return name;
    }
    virtual IWidget* getWidget()
    { 
        return NULL;
    }

    void* GetMainObject();

protected: 

    IMainWindow* m_pMainWindow;
    void* m_pMainObject;
    std::string name;

    ServiceManager* m_pServiceManager;
};


#endif //USER_INTERFACE_SERVICE_H