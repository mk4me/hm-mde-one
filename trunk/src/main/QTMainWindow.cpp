#include "QTMainWindow.h"


//--------------------------------------------------------------------------------------------------
void QTMainWindow::AddMenu(std::string menuName)
{
    //_fileMenu = _mainWindow->menuBar()->addMenu("&Test");
    std::string nameOut;
    std::string menuRootName;
    GetMenuNameString(menuName, nameOut);
    menuRootName = GetMenuRoot(menuName);

    if(menuRootName == "")
    {
        QMenu* menu = _mainWindow->menuBar()->addMenu(nameOut.c_str());
        _menuMap[menuName] = menu;
    }
    else
    {
        QMenu* menuRoot = _menuMap[menuRootName];
        if(!menuRoot)
            return;

        QMenu* menu = menuRoot->addMenu(nameOut.c_str());
        _menuMap[menuName] = menu;
    }
}

//--------------------------------------------------------------------------------------------------
void QTMainWindow::AddAction(std::string menuName, std::string actionName)
{
    QMenu* menu = _menuMap[menuName];
    if(!menu)
        return;

    QAction* action = new QAction(actionName.c_str(), _mainWindow);
    menu->addAction(action);
    _actionMap[actionName] = action;
}

//--------------------------------------------------------------------------------------------------
void QTMainWindow::AddAction( std::string menuName, std::string actionName, std::string slotMethodNameFromMainMenu)
{
    std::string slotMethod = "1";
    slotMethod.append(slotMethodNameFromMainMenu);

    QMenu* menu = _menuMap[menuName];
    if(!menu)
        return;

    QAction* action = new QAction(actionName.c_str(), _mainWindow);
    menu->addAction(action);
    _actionMap[actionName] = action;

    QObject::connect(action, SIGNAL(triggered()), _mainWindow ,slotMethod.c_str());
}

//--------------------------------------------------------------------------------------------------
QTMainWindow::QTMainWindow()
{

}

//--------------------------------------------------------------------------------------------------
QTMainWindow::QTMainWindow(void* object)
{
    _mainWindow = (QMainWindow*)(object);
}

//--------------------------------------------------------------------------------------------------
void QTMainWindow::GetMenuNameString( const std::string orginalStr, std::string& out )
{
    out = orginalStr;
    int lengthStr = orginalStr.length();
    if(!lengthStr)
        return;

    int indexStart = orginalStr.find_last_of("//");

    if(indexStart != 0xFFFFFFFF) 
    {
        indexStart++;
        out = orginalStr.substr(indexStart, (lengthStr - indexStart));
    }

    indexStart = orginalStr.find_last_of("\\");

    if(indexStart != 0xFFFFFFFF) 
    {
        indexStart++;
        out = orginalStr.substr(indexStart, (lengthStr - indexStart));
    }
}

//--------------------------------------------------------------------------------------------------
std::string QTMainWindow::GetMenuRoot( const std::string orginalStr )
{
    int lengthStr = orginalStr.length();
    if(!lengthStr)
        return "";

    int indexEnd = orginalStr.find_last_of("//");

    if(indexEnd != 0xFFFFFFFF) 
    {
        return orginalStr.substr(0, indexEnd);
    }

    indexEnd = orginalStr.find_last_of("\\");

    if(indexEnd != 0xFFFFFFFF) 
    {
        return orginalStr.substr(0, indexEnd);
    }

    return "";
}