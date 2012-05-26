#ifndef USER_INTERFACE_SERVICE_H
#define USER_INTERFACE_SERVICE_H

#include <string>
#include <core/IService.h>
#include <core/IUserInterface.h>

class UserInterfaceService: public core::IService, public IUserInterface
{
    UNIQUE_ID("{DACF9F0D-0B78-438C-B369-5F8EE087BF8C}", "User Interface Service");
public:
    UserInterfaceService();
    virtual ~UserInterfaceService();

public:
    //! \param mainWindow Wska�nik na g��wne okno.
    void setMainWindow(core::Window* mainWindow);

// IService
public:
    virtual QWidget* getWidget(std::vector<QObject*>& actions);
    virtual const std::string& getName() const;

    virtual void finalize() {};

// IUserInterface
public:
    //! \return Wska�nik na g��wne okno.
    virtual core::Window* getMainWindow();

private: 
    //! Wska�nik na g��wne okno.
    core::Window* mainWindow;
    //! Nazwa us�ugi.
    std::string name;
};


#endif //USER_INTERFACE_SERVICE_H