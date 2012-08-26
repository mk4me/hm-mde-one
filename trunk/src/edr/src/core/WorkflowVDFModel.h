/********************************************************************
    created:  2011/05/23
    created:  23:5:2011   20:58
    filename: WorkflowVDFModel.h
    author:   Mateusz Janiak

    purpose:
*********************************************************************/
#ifndef HEADER_GUARD___WORKFLOWVDFMODEL_H__
#define HEADER_GUARD___WORKFLOWVDFMODEL_H__

#ifdef IN
#undef IN
#endif

#ifdef OUT
#undef OUT
#endif

#include <QtCore/QObject>
#include <QtCore/QPoint>
#include <vdfmlib/osgVDFModel.h>

class WorkflowService;

class WorkflowVDFModel : public QObject, public osgVDF::osgVDFDefaultModel
{
    Q_OBJECT;
    friend class WorkflowService;

public:
    WorkflowVDFModel(osgViewer::View* view = 0, osgWidget::point_type width = 0.0f, osgWidget::point_type height = 0.0f,
        unsigned int nodeMask = 0, unsigned int flags = 0);

    ~WorkflowVDFModel();


public slots:
    void contextMenuRequestPosition(const QPoint & pos);

protected:
    virtual void showConnectionsDeleteContextMenu(const ConnectionsActionsMapping & toDelete, const ConnectionAction & leaveContextMenu, const osgWidget::XYCoord & pos);

private:
    volatile QPoint contextMenuPosition;
};

#endif  //  HEADER_GUARD___WORKFLOWVDFMODEL_H__

