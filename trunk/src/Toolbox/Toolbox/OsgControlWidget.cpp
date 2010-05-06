#include <iostream>
#include <osgViewer/Scene>
#include <osg/Group>
#include <stack> // TODO: Wszystko std musi byæ w przysz³oœci na STLport (sugestia)
#include "OsgControlWidget.h"

#include "ConfigurationService.h"

#include "ServiceManager.h"
#include "AnimationService.h"
#include "ObjectService.h"
#include "ModelWithSkeleton.h"

OsgControlWidget::OsgControlWidget(void):
  Ui::OsgTest()
, QWidget()
{
  setupUi(this); 

  connect(testButton, SIGNAL(clicked()), this, SLOT(makeTest()));
  connect(listWidget, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
 
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


void OsgControlWidget::selectionChanged()
{
/*

    QListWidgetItem* item;
    QList<QListWidgetItem*> selected = listWidget->itemClicked(item);

    std::string anim = "";

    QString	str = (*selected.begin())->text();
    anim= str.toAscii().constData();			

    ServiceManager::GetInstance()->RegisterServiceAs<AnimationService>(); 
    AnimationService* aServ = ServiceManager::GetInstance()->GetSystemServiceAs<AnimationService>();

    aServ->setSelectedAnimationName(anim);
*/

	QList<QListWidgetItem*> selected = listWidget->selectedItems();
	if (selected.begin() != selected.end())
	{
		std::string anim = "";

		QListWidgetItem* item = *selected.begin();
//		if (item->columnCount() > 1 )//&& !item->text(1).compare("Animation"))
//		{
			QString	str = item->text();
			anim		= str.toAscii().constData();			
//		}

		// get service
		ServiceManager::GetInstance()->RegisterServiceAs<AnimationService>(); 
		AnimationService* aServ = ServiceManager::GetInstance()->GetSystemServiceAs<AnimationService>();

		aServ->setSelectedAnimationName(anim);
	}
}



void OsgControlWidget::makeTest()
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


void OsgControlWidget::setScene(osgViewer::Scene *scene)
{
	// clear...
	clearScene(); 

	ServiceManager::GetInstance()->RegisterServiceAs<ObjectService>(); 
	ObjectService* objects = ServiceManager::GetInstance()->GetSystemServiceAs<ObjectService>();

	CModelWithSkeleton* model = dynamic_cast<CModelWithSkeleton*>(objects->getModel());
	if (model)
	{
		std::map<std::string, CAnimation*>* animations = model->getAnimations();

        // R.Zowal not neded naw 
//		QListWidgetItem *rootListItem = new QListWidgetItem(listWidget);
//		rootTreeItem->setText(0, "Animations");
		for (std::map<std::string, CAnimation*>::iterator i = animations->begin(); i != animations->end(); ++i)
		{
			QListWidgetItem *nodeListItem = new QListWidgetItem(listWidget); 
			nodeListItem->setText(i->first.c_str()); 
	//		nodeListItem->setText(tr("Animation"));
		}
	
    }

}

void OsgControlWidget::clearScene()
{
    listWidget->reset();
    listWidget->clear();

/*
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
  }*/

}

/*
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

*/
