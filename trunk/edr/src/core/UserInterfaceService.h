#ifndef USER_INTERFACE_SERVICE_H
#define USER_INTERFACE_SERVICE_H

#include <string>
#include <osg/Node>
#include <core/IService.h>
#include <core/IUserInterface.h>

class UserInterfaceService: public IService, public IUserInterface
{
    UNIQUE_ID('UI','SRVC');
public:
    UserInterfaceService();
    virtual ~UserInterfaceService();

public:
    //! \param mainWindow Wskaünik na g≥Ûwne okno.
    void setMainWindow(core::Window* mainWindow);

// IService
public:
    virtual AsyncResult init(IServiceManager* serviceManager, osg::Node* sceneRoot);
    virtual AsyncResult loadData(IServiceManager* serviceManager, core::IDataManager* dataManager);
    virtual AsyncResult update(double time, double timeDelta);
    virtual AsyncResult lateUpdate(double time, double timeDelta);
    virtual AsyncResult compute();
    virtual IWidget* getWidget();
    virtual const std::string& getName() const;

// IUserInterface
public:
    //! \return Wskaünik na g≥Ûwne okno.
    virtual core::Window* getMainWindow();

private: 
    //! Wskaünik na g≥Ûwne okno.
    core::Window* mainWindow;
    //! Nazwa us≥ugi.
    std::string name;
};


#endif //USER_INTERFACE_SERVICE_H