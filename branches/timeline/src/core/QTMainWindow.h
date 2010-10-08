#ifndef QT_MAINWINDOW_H
#define QT_MAINWINDOW_H

#include <core/QtMainWindowInterface.h>
#include <QtGui/QMainWindow>
#include <QtGui/QtGui>
#include <QtGui/QMenu>
#include <QtCore/QDir>
#include <QtGui/QAction>
#include <QtCore/QVector>

#include <map>

class QTMainWindow : public IQtMainWindow
{
public:
    QTMainWindow();
    QTMainWindow(void* object);
    virtual void AddMenu(std::string menuName);
    virtual void AddAction(std::string menuName, std::string actionName);
    virtual void AddAction(std::string menuName, std::string actionName, std::string slotMethodNameFromMainMenu);
protected:
private:
    void GetMenuNameString(const std::string orginalStr, std::string& out);
    std::string GetMenuRoot(const std::string orginalStr);
    QMainWindow* _mainWindow;
    std::map<std::string, QMenu* > _menuMap;
    std::map<std::string, QAction* > _actionMap;
};

#endif //QT_MAINWINDOW_H