#ifndef GRID_WIDGET_H
#define GRID_WIDGET_H

#include <QtGui/QWidget>
#include <osgViewer/Scene>
#include <osg/Group>
#include <osg/Geode>
#include <osg/NodeVisitor>
#include <string>

#include "ui_GridWidget.h"

class GridWidget: public QWidget, Ui::GridWidget
{
    Q_OBJECT
public:
    GridWidget(void);
    ~GridWidget(void);

    void setScene(osgViewer::Scene *scene); 
    void clearScene(); 




    public slots: 

        virtual void selectionChanged();
        virtual void makeTest();
        void addGroupToTreeView(osg::Group *group, QTreeWidgetItem *parentTreeItem);
        void addGeodeToTreeView(osg::Geode *geode, QTreeWidgetItem *parentTreeItem);
};


#endif   //GRID_WIDGET_H
