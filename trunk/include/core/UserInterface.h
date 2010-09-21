#ifndef IUSERINTERFACE_H
#define IUSERINTERFACE_H

#include "MainWindow.h"

class IUserInterface
{
public:
    virtual void AddMenu(std::string menuName) = 0;
    virtual void AddAction(std::string menuName, std::string actionName) = 0;
    virtual void AddAction(std::string menuName, std::string actionName, std::string slotMethodNameFromMainMenu) = 0;

    virtual void InicializeServices(std::string typeName, void *object) = 0;
};



#endif //IUSERINTERFACE_H