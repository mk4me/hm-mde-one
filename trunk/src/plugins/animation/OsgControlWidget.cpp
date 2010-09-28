#include "OsgControlWidget.h"

#include <iostream>
#include <osgViewer/Scene>
#include <osg/Group>
#include <stack> // TODO: Wszystko std musi byæ w przysz³oœci na STLport (sugestia)

#include <core/IServiceManager.h>
#include <plugins/animation/AnimationService.h>

//--------------------------------------------------------------------------------------------------
OsgControlWidget::OsgControlWidget(void):
Ui::OsgTest()
, QWidget()
{
    setupUi(this); 

    connect(testButton, SIGNAL(clicked()), this, SLOT(MakeTest()));
    connect(listWidget, SIGNAL(itemSelectionChanged()), this, SLOT(SelectionChanged()));

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

        m_pAnimationService->SetSelectedAnimationName(anim);
    }
}

//--------------------------------------------------------------------------------------------------
void OsgControlWidget::MakeTest()
{	
    if (m_pAnimationService->GetSelectedAnimationName().length())
    {
        m_pAnimationService->PlayAnimation(m_pAnimationService->GetSelectedAnimationName());
    }
}

//--------------------------------------------------------------------------------------------------
void OsgControlWidget::SetScene(osgViewer::Scene *scene)
{
//     // clear...
//     ClearScene(); 
//     AnimationService* animationService = pServiceManager->GetSystemServiceAs<AnimationService>();
// 
//     std::map<std::string, Animation*>* animations = animationService->GetAnimations();
// 
//     for (std::map<std::string, Animation*>::iterator i = animations->begin(); i != animations->end(); ++i)
//     {
//         QListWidgetItem *nodeListItem = new QListWidgetItem(listWidget); 
//         nodeListItem->setText(i->first.c_str()); 
//     }
}

//--------------------------------------------------------------------------------------------------
void OsgControlWidget::SetScene( osgViewer::Scene *scene, IServiceManager *pServiceManager )
{
  int q = 'xyzw';
  UniqueId cid('TEST','STFU');
  UniqueId cid2('TEST','STF1');
    // clear...
    ClearScene(); 
    m_pAnimationService = dynamic_cast<AnimationService*>(pServiceManager->GetSystemService(AnimationService::CLASS_ID));

    std::map<std::string, Animation*>* animations = m_pAnimationService->GetAnimations();

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
