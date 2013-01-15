#ifndef USER_INTERFACE_SERVICE_H
#define USER_INTERFACE_SERVICE_H

#include <string>
#include <core/IService.h>
#include <core/IUserInterface.h>

class UserInterfaceService: public plugin::IService, public IUserInterface
{
    UNIQUE_ID("{DACF9F0D-0B78-438C-B369-5F8EE087BF8C}", "UserInterfaceService", "User Interface Service");
public:
    UserInterfaceService();
    virtual ~UserInterfaceService();

public:
    //! \param mainWindow Wskaźnik na główne okno.
    void setMainWindow(core::Window* mainWindow);

// IService
public:
    virtual QWidget* getWidget(std::vector<QObject*>& actions);

    virtual void finalize() {};

// IUserInterface
public:
    //! \return Wskaźnik na główne okno.
    virtual core::Window* getMainWindow();

private: 
    //! Wskaźnik na główne okno.
    core::Window* mainWindow;
};


#endif //USER_INTERFACE_SERVICE_H
