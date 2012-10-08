#include "CorePCH.h"
#include "WorkflowVDFModel.h"
#include "WorkflowVDFConnDelContextMenu.h"
#include <QtGui/QWidget>

WorkflowVDFModel::WorkflowVDFModel(osgViewer::View* view, osgWidget::point_type width,
    osgWidget::point_type height, unsigned int nodeMask, unsigned int flags)
    : osgVDF::osgVDFDefaultModel(view, width, height, nodeMask, flags)
{

}

WorkflowVDFModel::~WorkflowVDFModel()
{

}

void WorkflowVDFModel::contextMenuRequestPosition(const QPoint & pos)
{
    const_cast<QPoint&>(contextMenuPosition) = ((QWidget*)sender())->mapToGlobal(pos);
}

void WorkflowVDFModel::showConnectionsDeleteContextMenu(const ConnectionsActionsMapping & toDelete, const ConnectionAction & leaveContextMenu, const osgWidget::XYCoord & pos)
{
    QMenu * newMenu = new WorkflowVDFConnDelContextMenu(toDelete, leaveContextMenu);

    newMenu->popup(const_cast<QPoint&>(contextMenuPosition));
}
