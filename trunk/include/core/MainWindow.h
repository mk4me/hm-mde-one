#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <string>

class IMainWindow
{
public:
    virtual void AddMenu(std::string menuName) = 0;
    virtual void AddAction(std::string menuName, std::string actionName) = 0;
    virtual void AddAction(std::string menuName, std::string actionName, std::string slotMethodNameFromMainMenu) = 0;
protected:
private:
};


#endif //MAINWINDOW_H