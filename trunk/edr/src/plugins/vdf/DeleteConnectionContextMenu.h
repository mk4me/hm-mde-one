/********************************************************************
    created:  2011/04/26
    created:  26:4:2011   19:47
    filename: DeleteConnectionContextMenu.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___DELETECONNECTIONCONTEXTMENU_H__
#define HEADER_GUARD___DELETECONNECTIONCONTEXTMENU_H__

#include "VdfQtModel.h"
#include <Qt/qmenu.h>

class DeleteConnectionContextMenu : public QMenu
{
    Q_OBJECT;

public:
    DeleteConnectionContextMenu(const osgVDF::osgVDFBaseModel::CONNECTIONS_ACTIONS_MAP & toDelete, const osgVDF::osgVDFBaseModel::CONNECTION_ACTION & leaveContextMenu);

protected:

    virtual void leaveEvent(QEvent * ev);

private slots:

    void onHover();
    void onTrigger(bool trigger);

private:

    osgVDF::osgVDFBaseModel::CONNECTION_ACTION leaveAction;
    std::map<QAction*, osgVDF::osgVDFBaseModel::CONNECTION_ACTIONS> actions;

};

#endif  //  HEADER_GUARD___DELETECONNECTIONCONTEXTMENU_H__