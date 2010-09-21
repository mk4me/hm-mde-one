#ifndef QT_MAINWINDOW_INTERFACE_H
#define QT_MAINWINDOW_INTERFACE_H

#include "MainWindow.h"


class IQtMainWindow: public IMainWindow
{
public:
    virtual void AddMenu(std::string menuName) = 0;
    virtual void AddAction(std::string menuName, std::string actionName) = 0;
    virtual void AddAction(std::string menuName, std::string actionName, std::string slotMethodNameFromMainMenu) = 0;
protected:
private:
};

#endif //QT_MAINWINDOW_INTERFACE_H