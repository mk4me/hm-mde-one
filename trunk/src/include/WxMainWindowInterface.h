#ifndef Wx_MAINWINDOW_INTERFACE_H
#define Wx_MAINWINDOW_INTERFACE_H

#include "MainWindow.h"


class IWxMainWindow: public IMainWindow
{
public:
    virtual void AddMenu();
    virtual void AddAction();
protected:
private:
};

#endif //Wx_MAINWINDOW_INTERFACE_H