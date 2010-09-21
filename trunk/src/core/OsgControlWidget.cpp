#include <iostream>
#include <osgViewer/Scene>
#include <osg/Group>
#include <stack> // TODO: Wszystko std musi byæ w przysz³oœci na STLport (sugestia)
#include "OsgControlWidget.h"

#include "ServiceManager.h"
#include "AnimationService.h"

//--------------------------------------------------------------------------------------------------
OsgControlWidget::OsgControlWidget(void):
Ui::OsgTest()
, QWidget()
{
    setupUi(this); 

    connect(testButton, SIGNAL(clicked()), this, SLOT(MakeTest()));
    connect(listWidget, SIGNAL(itemSelectionChanged()), this, SLOT(SelectionChanged()));

    /*
    QTreeWidgetItem *cities = new QTreeWidgetItem(sceneGraphWidget);
    cities->setText(0, tr("Cities"));
    QTreeWidgetItem *osloItem = new QTreeWidgetItem(cities);
    osloItem->setText(0, tr("Oslo"));
    osloItem->setText(1, tr("Yes"));
    /**/
}

//--------------------------------------------------------------------------------------------------
OsgControlWidget::~OsgControlWidget(void)
{
    ClearScene(); 
}

//--------------------------------------------------------------------------------------------------
void OsgControlWidget::SelectionChanged()
{
    QList<QListWidgetItem*> selected = listWidget->selectedItems();
    if (selected.begin() != selected.end())
    {
        std::string anim = "";

        QListWidgetItem* item = *selected.begin();

        QString	str = item->text();
        anim = str.toAscii().constData();			

        AnimationService* aServ = ServiceManager::GetInstance()->GetSystemServiceAs<AnimationService>();
        aServ->SetSelectedAnimationName(anim);
    }
}

//--------------------------------------------------------------------------------------------------
void OsgControlWidget::MakeTest()
{	
    // get service
    AnimationService* animServ = ServiceManager::GetInstance()->GetSystemServiceAs<AnimationService>();

    if (animServ->GetSelectedAnimationName().length())
    {
        animServ->PlayAnimation(animServ->GetSelectedAnimationName());
    }
}

//--------------------------------------------------------------------------------------------------
void OsgControlWidget::SetScene(osgViewer::Scene *scene)
{
    // clear...
    ClearScene(); 
    AnimationService* animationService = ServiceManager::GetInstance()->GetSystemServiceAs<AnimationService>();

    std::map<std::string, Animation*>* animations = animationService->GetAnimations();

    for (std::map<std::string, Animation*>::iterator i = animations->begin(); i != animations->end(); ++i)
    {
        QListWidgetItem *nodeListItem = new QListWidgetItem(listWidget); 
        nodeListItem->setText(i->first.c_str()); 
    }
}

//--------------------------------------------------------------------------------------------------
void OsgControlWidget::ClearScene()
{
    listWidget->reset();
    listWidget->clear();
}

/*
//--------------------------------------------------------------------------------------------------
void OsgControlWidget::AddGroupToTreeView(osg::Group *group, QTreeWidgetItem *parentTreeItem)
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

//--------------------------------------------------------------------------------------------------
void OsgControlWidget::AddGeodeToTreeView(osg::Geode *geode, QTreeWidgetItem *parentTreeItem)
{
    QTreeWidgetItem *nodeTreeItem = new QTreeWidgetItem(parentTreeItem); 
    nodeTreeItem->setText(0, geode->getName().c_str()); 
    nodeTreeItem->setText(1, tr("Geode"));
}
/**/
