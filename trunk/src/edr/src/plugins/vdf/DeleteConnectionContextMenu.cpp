#include "DeleteConnectionContextMenu.h"
#include <Qt/qaction.h>

DeleteConnectionContextMenu::DeleteConnectionContextMenu(const osgVDF::osgVDFBaseModel::CONNECTIONS_ACTIONS_MAP & toDelete, const osgVDF::osgVDFBaseModel::CONNECTION_ACTION & leaveContextMenu)
    : QMenu(), leaveAction(leaveContextMenu)
{
    for(auto it = toDelete.begin(); it != toDelete.end(); it++){
        std::string actionText("Delete " + it->first);
        QAction * newAction = new QAction(actionText.c_str(), this);
        actions[newAction] = it->second;
        connect(newAction, SIGNAL(triggered(bool)), this, SLOT(onTrigger(bool)));
        connect(newAction, SIGNAL(hovered()), this, SLOT(onHover()));
        addAction(newAction);
    }
}

void DeleteConnectionContextMenu::onHover()
{
    actions[(QAction*)sender()].first();
}

void DeleteConnectionContextMenu::onTrigger(bool trigger)
{
    actions[(QAction*)sender()].second();
}

void DeleteConnectionContextMenu::leaveEvent(QEvent * ev)
{
    leaveAction();
    QMenu::leaveEvent(ev);
}
