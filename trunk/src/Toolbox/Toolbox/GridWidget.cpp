#include "GridWidget.h"

#include <iostream>
#include <osgViewer/Scene>
#include <osg/Group>
#include <stack> // TODO: Wszystko std musi by� w przysz�o�ci na STLport (sugestia)

#include "ConfigurationService.h"

#include "ServiceManager.h"
#include "AnimationService.h"
#include "ObjectService.h"
#include "ModelWithSkeleton.h"

GridWidget::GridWidget(void):
Ui::GridWidget()
, QWidget()
{
    setupUi(this); 

    connect(testButton, SIGNAL(clicked()), this, SLOT(makeTest()));
    connect(sceneGraphWidget, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));

    /*
    QTreeWidgetItem *cities = new QTreeWidgetItem(sceneGraphWidget);
    cities->setText(0, tr("Cities"));
    QTreeWidgetItem *osloItem = new QTreeWidgetItem(cities);
    osloItem->setText(0, tr("Oslo"));
    osloItem->setText(1, tr("Yes"));
    /**/
}

GridWidget::~GridWidget(void)
{
    clearScene(); 
}

void GridWidget::selectionChanged()
{
    //
    //tymczasowo wywalic, poniewa� nie mozemy wp�ywac na animacje, ale co najwyzej na pod�wietlenie grid�w itp.
    //TODO: przypisa� now� funkcjonalno��.

    QList<QTreeWidgetItem*> selected = sceneGraphWidget->selectedItems();
    if (selected.begin() != selected.end())
    {
      std::string group = "";

      QTreeWidgetItem* item = *selected.begin();
      if (item->columnCount() > 1)
      {
            QString	str = (*selected.begin())->text(0);
            group		= str.toAscii().constData();	


            ServiceManager::GetInstance()->RegisterServiceAs<ObjectService>(); 
            ObjectService* objects = ServiceManager::GetInstance()->GetSystemServiceAs<ObjectService>();

            CModelWithSkeleton* model = dynamic_cast<CModelWithSkeleton*>(objects->getModel());
            if (model)
            {
                model->selectGroup(group);
                model->updateSkeletonMesh();
            }
      }
        // get service
    }

}

void GridWidget::makeTest()
{	
    // get service
    ServiceManager::GetInstance()->RegisterServiceAs<ObjectService>(); 
    ObjectService* objects = ServiceManager::GetInstance()->GetSystemServiceAs<ObjectService>();

    CModelWithSkeleton* model = dynamic_cast<CModelWithSkeleton*>(objects->getModel());
    if (model)
    {
        // using AnimationService is more elegant than taking it directly from list
        ServiceManager::GetInstance()->RegisterServiceAs<AnimationService>(); 
        AnimationService* animServ = ServiceManager::GetInstance()->GetSystemServiceAs<AnimationService>();

        if (animServ->getSelectedAnimationName().length())
        {
            CAnimation* animation = model->getAnimation(animServ->getSelectedAnimationName());
            if (animation)
                animation->play(model);
        }
    }

    //std::cout << "Wczytywanie Lua " << 3.1415f << " :D" << std::endl; 
    //QString path = qApp->applicationDirPath();
    //path.append("/"); 
    //path.append("Toolbox_config.ini"); 
    //ConfigurationService *config = new ConfigurationService(); 
    //config->loadConfiguration( std::string(path.toUtf8()) ); 
}

void GridWidget::setScene(osgViewer::Scene *scene)
{
    // clear...
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

void GridWidget::clearScene()
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

void GridWidget::addGroupToTreeView(osg::Group *group, QTreeWidgetItem *parentTreeItem)
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

void GridWidget::addGeodeToTreeView(osg::Geode *geode, QTreeWidgetItem *parentTreeItem)
{
    QTreeWidgetItem *nodeTreeItem = new QTreeWidgetItem(parentTreeItem); 
    nodeTreeItem->setText(0, geode->getName().c_str()); 
    nodeTreeItem->setText(1, tr("Geode"));

}

