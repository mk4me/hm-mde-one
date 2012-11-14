#include "VdfQtModel.h"
#include "DeleteConnectionContextMenu.h"
#include <QtGui/QWidget>

VdfQtModel::VdfQtModel(osgViewer::View* view, osgWidget::point_type width, osgWidget::point_type height,
    unsigned int nodeMask, unsigned int flags) : osgVDF::osgVDFModel<osgVDF::osgVDFNode, osgVDF::osgVDFPin<osgVDF::BezierConnectionFactory> >(view, width, height, nodeMask, flags)
{

}

void VdfQtModel::contextMenuRequestPosition(const QPoint & pos)
{
    //contextMenuPosition = ((QWidget*)sender())->mapToGlobal(pos);
    const_cast<QPoint&>(contextMenuPosition) = ((QWidget*)sender())->mapToGlobal(pos);
}

 void VdfQtModel::showConnectionsDeleteContextMenu(const CONNECTIONS_ACTIONS_MAP & toDelete, const CONNECTION_ACTION & leaveContextMenu, const osgWidget::XYCoord & pos)
 {
     QMenu * newMenu = new DeleteConnectionContextMenu(toDelete, leaveContextMenu);

     newMenu->popup(const_cast<QPoint&>(contextMenuPosition));
 }
