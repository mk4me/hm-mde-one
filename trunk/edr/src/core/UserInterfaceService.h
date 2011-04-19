#ifndef USER_INTERFACE_SERVICE_H
#define USER_INTERFACE_SERVICE_H

#include <string>
#include <osg/Node>
#include <core/IService.h>
#include <core/IUserInterface.h>

class UserInterfaceService: public core::IService, public IUserInterface
{
    UNIQUE_ID('UI','SRVC');
public:
    UserInterfaceService();
    virtual ~UserInterfaceService();

public:
    //! \param mainWindow Wska�nik na g��wne okno.
    void setMainWindow(core::Window* mainWindow);

// IService
public:
    virtual QWidget* getWidget();
    virtual const std::string& getName() const;

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