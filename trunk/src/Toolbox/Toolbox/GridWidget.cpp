#include "GridWidget.h"

#include <iostream>
#include <osgViewer/Scene>
#include <osg/Group>
#include <stack> // TODO: Wszystko std musi byæ w przysz³oœci na STLport (sugestia)

#include "ConfigurationService.h"

#include "ServiceManager.h"
#include "AnimationService.h"
#include "ObjectService.h"
#include "ModelWithSkeleton.h"

//--------------------------------------------------------------------------------------------------
GridWidget::GridWidget(void):
  Ui::GridWidget()
, QWidget()
{
    setupUi(this); 

    connect(testButton, SIGNAL(clicked()), this, SLOT(MakeTest()));
    connect(sceneGraphWidget, SIGNAL(itemSelectionChanged()), this, SLOT(SelectionChanged()));

    /*
    QTreeWidgetItem *cities = new QTreeWidgetItem(sceneGraphWidget);
    cities->setText(0, tr("Cities"));
    QTreeWidgetItem *osloItem = new QTreeWidgetItem(cities);
    osloItem->setText(0, tr("Oslo"));
    osloItem->setText(1, tr("Yes"));
    /**/
}

//--------------------------------------------------------------------------------------------------
GridWidget::~GridWidget(void)
{
    ClearScene(); 
}

//--------------------------------------------------------------------------------------------------
void GridWidget::SelectionChanged()
{
    //
    //tymczasowo wywalic, poniewa¿ nie mozemy wp³ywac na animacje, ale co najwyzej na podœwietlenie gridów itp.
    //TODO: przypisaæ now¹ funkcjonalnoœæ.

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

            ModelWithSkeleton* model = dynamic_cast<ModelWithSkeleton*>(objects->GetModel());
            if (model)
            {
                model->SelectGroup(group);
                model->UpdateSkeletonMesh();
            }
      }
        // get service
    }
}

//--------------------------------------------------------------------------------------------------
void GridWidget::MakeTest()
{	
    // get service
    ServiceManager::GetInstance()->RegisterServiceAs<ObjectService>(); 
    ObjectService* objects = ServiceManager::GetInstance()->GetSystemServiceAs<ObjectService>();

    ModelWithSkeleton* model = dynamic_cast<ModelWithSkeleton*>(objects->GetModel());
    if (model)
    {
        // using AnimationService is more elegant than taking it directly from list
        ServiceManager::GetInstance()->RegisterServiceAs<AnimationService>(); 
        AnimationService* animServ = ServiceManager::GetInstance()->GetSystemServiceAs<AnimationService>();

        if (animServ->GetSelectedAnimationName().length())
        {
            Animation* animation = model->GetAnimation(animServ->GetSelectedAnimationName());
            if (animation)
                animation->Play(model);
        }
    }

    //std::cout << "Wczytywanie Lua " << 3.1415f << " :D" << std::endl; 
    //QString path = qApp->applicationDirPath();
    //path.append("/"); 
    //path.append("Toolbox_config.ini"); 
    //ConfigurationService *config = new ConfigurationService(); 
    //config->loadConfiguration( std::string(path.toUtf8()) ); 
}

//--------------------------------------------------------------------------------------------------
void GridWidget::SetScene(osgViewer::Scene *scene)
{
    // clear...
    ClearScene(); 

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
                AddGroupToTreeView(child->asGroup(), rootTreeItem); 
            }
            else if(child->asGeode())
            {
                AddGeodeToTreeView(child->asGeode(), rootTreeItem); 
            }

        }
    }
}

//--------------------------------------------------------------------------------------------------
void GridWidget::ClearScene()
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

//--------------------------------------------------------------------------------------------------
void GridWidget::AddGroupToTreeView(osg::Group *group, QTreeWidgetItem *parentTreeItem)
{
    QTreeWidgetItem *nodeTreeItem = new QTreeWidgetItem(parentTreeItem); 
    nodeTreeItem->setText(0, group->getName().c_str()); 
    nodeTreeItem->setText(1, tr("Group"));

    int childCount = group->getNumChildren(); 
    for (int i=0; i<childCount; ++i)
    {
        osg::Node *child = group->getChild(i); 
        if(child->asGroup() != 0)
            AddGroupToTreeView(child->asGroup(), nodeTreeItem); 
    }
}

//--------------------------------------------------------------------------------------------------
void GridWidget::AddGeodeToTreeView(osg::Geode *geode, QTreeWidgetItem *parentTreeItem)
{
    QTreeWidgetItem *nodeTreeItem = new QTreeWidgetItem(parentTreeItem); 
    nodeTreeItem->setText(0, geode->getName().c_str()); 
    nodeTreeItem->setText(1, tr("Geode"));
}
