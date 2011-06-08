#include "CorePCH.h"
#include "WorkflowVDFConnDelContextMenu.h"
#include <QtGui/QAction>

WorkflowVDFConnDelContextMenu::WorkflowVDFConnDelContextMenu(const osgVDF::osgVDFBaseModel::ConnectionsActionsMapping & toDelete, const osgVDF::osgVDFBaseModel::ConnectionAction & leaveContextMenu)
    : QMenu(), leaveAction(leaveContextMenu)
{
    for(auto it = toDelete.begin(); it != toDelete.end(); it++){
        std::string actionText("Delete " + it->first);
        QAction * newAction = new QAction(actionText.c_str(), this);
        actions[newAction] = it->second;
        bool conn = connect(newAction, SIGNAL(triggered(bool)), this, SLOT(onTrigger(bool)));
        conn = connect(newAction, SIGNAL(hovered()), this, SLOT(onHover()));
        addAction(newAction);
    }
}

void WorkflowVDFConnDelContextMenu::onHover()
{
    actions[(QAction*)sender()].first();
}

void WorkflowVDFConnDelContextMenu::onTrigger(bool trigger)
{
    actions[(QAction*)sender()].second();
}

void WorkflowVDFConnDelContextMenu::leaveEvent(QEvent * ev)
{
    leaveAction();
    QMenu::leaveEvent(ev);
}