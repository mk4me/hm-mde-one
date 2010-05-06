#include <iostream>
#include <osgViewer/Scene>
#include <osg/Group>
#include <stack> // TODO: Wszystko std musi byæ w przysz³oœci na STLport (sugestia)
#include "OsgControlWidget.h"



OsgControlWidget::OsgControlWidget(void):
  Ui::OsgTest()
, QWidget()
{
    setupUi(this); 

    connect(testButton, SIGNAL(clicked()), this, SLOT(makeTest())); 

    /*
    QTreeWidgetItem *cities = new QTreeWidgetItem(sceneGraphWidget);
    cities->setText(0, tr("Cities"));
    QTreeWidgetItem *osloItem = new QTreeWidgetItem(cities);
    osloItem->setText(0, tr("Oslo"));
    osloItem->setText(1, tr("Yes"));
    /**/
}

OsgControlWidget::~OsgControlWidget(void)
{
  clearScene(); 
}

void OsgControlWidget::makeTest()
{
    std::cout << "hahahha test wykonany! " << 3.1415f << " :D" << std::endl; 
}

void OsgControlWidget::setScene(osgViewer::Scene *scene)
{
    clearScene(); 

    osg::Node *topNode = scene->getSceneData(); 
    osg::Group *topGroup = 0; 
    if(topNode)
        topGroup = topNode->asGroup(); 

    if(topGroup != 0)
    {
        QTreeWidgetItem *rootTreeItem = new QTreeWidgetItem(sceneGraphWidget);
        rootTreeItem->setText(0, topGroup->getName().c_str());

        int childCount = topGroup->getNumChildren(); 
        for (int i=0; i<childCount; ++i)
        {
            osg::Node *child = topGroup->getChild(i); 
            if(child->asGroup() != 0)
            {
                addGroupToTreeView(child->asGroup(), rootTreeItem); 
            }
            else if(child->asGeode())
            {
                addGeodeToTreeView(child->asGeode(), rootTreeItem); 
            }

        }
    }
}

void OsgControlWidget::clearScene()
{
    std::stack<QTreeWidgetItem *> itemStack; 

    if(!sceneGraphWidget)
        return; 
    int childCount = sceneGraphWidget->topLevelItemCount(); 

    for(int i=0; i<childCount; ++i)
        itemStack.push(sceneGraphWidget->topLevelItem(i)); 

    while (itemStack.size() != 0)
    {
        QTreeWidgetItem *item = itemStack.top(); 
        childCount = item->childCount(); 
        for (int i=0; i<childCount; ++i)
        {
            itemStack.push( item->child(i) ); 
        }
        if(childCount == 0)
        {
            if(item->parent())
                item->parent()->removeChild(item); 
            itemStack.pop(); 
            delete item; 
        }
    }
}

void OsgControlWidget::addGroupToTreeView(osg::Group *group, QTreeWidgetItem *parentTreeItem)
{
    QTreeWidgetItem *nodeTreeItem = new QTreeWidgetItem(parentTreeItem); 
    nodeTreeItem->setText(0, group->getName().c_str()); 
    nodeTreeItem->setText(1, tr("Group"));

    int childCount = group->getNumChildren(); 
    for (int i=0; i<childCount; ++i)
    {
        osg::Node *child = group->getChild(i); 
        if(child->asGroup() != 0)
            addGroupToTreeView(child->asGroup(), nodeTreeItem); 
    }
}

void OsgControlWidget::addGeodeToTreeView(osg::Geode *geode, QTreeWidgetItem *parentTreeItem)
{
    QTreeWidgetItem *nodeTreeItem = new QTreeWidgetItem(parentTreeItem); 
    nodeTreeItem->setText(0, geode->getName().c_str()); 
    nodeTreeItem->setText(1, tr("Geode"));

}

