/********************************************************************
    created:  2011/05/23
    created:  23:5:2011   21:09
    filename: WorkflowVDFConnDelContextMenu.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___WORKFLOWVDFCONNDELCONTEXTMENU_H__
#define HEADER_GUARD___WORKFLOWVDFCONNDELCONTEXTMENU_H__

#include "WorkflowVDFModel.h"
#include <QtGui//QMenu>

class WorkflowVDFConnDelContextMenu : public QMenu
{
    Q_OBJECT;

public:
    WorkflowVDFConnDelContextMenu(const osgVDF::osgVDFBaseModel::ConnectionsActionsMapping & toDelete, const osgVDF::osgVDFBaseModel::ConnectionAction & leaveContextMenu);

protected:

    virtual void leaveEvent(QEvent * ev);

private slots:

    void onHover();
    void onTrigger(bool trigger);

private:

    osgVDF::osgVDFBaseModel::ConnectionAction leaveAction;
    std::map<QAction*, osgVDF::osgVDFBaseModel::ConnectionActions> actions;

};

#endif  //  HEADER_GUARD___WORKFLOWVDFCONNDELCONTEXTMENU_H__