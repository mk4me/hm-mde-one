/********************************************************************
    created:  2011/04/26
    created:  26:4:2011   18:13
    filename: VdfQtModel.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD__VDFQTMODEL_H__
#define HEADER_GUARD__VDFQTMODEL_H__

#include <QtCore/QObject>
#include <vdfmlib/osgVDFModel.h>
#include <vdfmlib/osgVDFNode.h>
#include <vdfmlib/osgVDFPin.h>

class VdfWidget;

class DeleteConnectionContextMenu;

class VdfQtModel : public QObject, public osgVDF::osgVDFModel<osgVDF::osgVDFNode, osgVDF::osgVDFPin<osgVDF::BezierConnectionFactory> >
{
    Q_OBJECT
    friend class DeleteConnectionContextMenu;

public:
    VdfQtModel(osgViewer::View* view = 0, osgWidget::point_type width = 0.0f, osgWidget::point_type height = 0.0f,
        unsigned int nodeMask = 0, unsigned int flags = 0);

    virtual dflm::ConnPtr connect(dflm::PinPtr src, dflm::PinPtr dest)
    {
        dflm::ConnPtr ret(osgVDFBaseModel::connect(src,dest));

        if(ret != nullptr){

        }

        //do true connection here


        return ret;
    }

    virtual void disconnect(dflm::ConnPtr connection)
    {
        osgVDFBaseModel::disconnect(connection);

    }

public slots:
    void contextMenuRequestPosition(const QPoint & pos);

protected:

    virtual void showConnectionsDeleteContextMenu(const CONNECTIONS_ACTIONS_MAP & toDelete, const CONNECTION_ACTION & leaveContextMenu, const osgWidget::XYCoord & pos);

private:

    volatile QPoint contextMenuPosition;
};

#endif  //  HEADER_GUARD__VDFQTMODEL_H__

