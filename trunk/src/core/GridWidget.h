#ifndef GRID_WIDGET_H
#define GRID_WIDGET_H

#include <QtGui/QWidget>
#include <osgViewer/Scene>
#include <osg/Group>
#include <osg/Geode>
#include <osg/NodeVisitor>
#include <string>

#include <core/IModel.h>
#include <core/Skeleton.h>

#include "ui_GridWidget.h"

class GridWidget: public QWidget, Ui::GridWidget
{
    Q_OBJECT
public:
    GridWidget(void);
    ~GridWidget(void);

    void SetScene(osg::Node *scene); 
    void ClearScene(); 

public slots: 
    virtual void SelectionChanged();
    virtual void MakeTest();
    void AddChildToTreeView(Bone* bone, QTreeWidgetItem *parentTreeItem);

    void AddGroupToTreeView(osg::Group *group, QTreeWidgetItem *parentTreeItem);
    void AddGeodeToTreeView(osg::Geode *geode, QTreeWidgetItem *parentTreeItem);


};


#endif   //GRID_WIDGET_H
