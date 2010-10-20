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
    //followTimelineCheckbox->setChecked(true);

//     connect(testButton, SIGNAL(clicked()), this, SLOT(MakeTest()));
//     connect(listWidget, SIGNAL(itemSelectionChanged()), this, SLOT(SelectionChanged()));

}

//--------------------------------------------------------------------------------------------------
OsgControlWidget::~OsgControlWidget(void)
{
    ClearScene(); 
}

//--------------------------------------------------------------------------------------------------
void OsgControlWidget::animationSelectionChanged()
{
    QList<QListWidgetItem*> selected = animationList->selectedItems();
    if (selected.begin() != selected.end())
    {
        std::string anim = "";
        QListWidgetItem* item = *selected.begin();
        QString	str = item->text();
        anim = str.toAscii().constData();			
        if ( anim == "[none]")   {
            m_pAnimationService->SetSelectedAnimationName("");
            m_pAnimationService->PlayAnimation("");
        } else {
            m_pAnimationService->SetSelectedAnimationName(anim);
            m_pAnimationService->PlayAnimation(anim);
        }
    }
}
//------------------------------------------------------------------------------

void OsgControlWidget::followTimelineChecked( int checked )
{
    m_pAnimationService->setFollowTimeline( checked != 0 );
}

//--------------------------------------------------------------------------------------------------
// void OsgControlWidget::MakeTest()
// {	
//     if (m_pAnimationService->GetSelectedAnimationName().length())
//     {
//         m_pAnimationService->PlayAnimation(m_pAnimationService->GetSelectedAnimationName());
//     }
// }

//--------------------------------------------------------------------------------------------------
void OsgControlWidget::SetScene(osg::Node *scene)
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
void OsgControlWidget::SetScene( osg::Node *scene, IServiceManager *pServiceManager )
{
    // clear...
    ClearScene(); 
    m_pAnimationService = dynamic_cast<AnimationService*>(pServiceManager->getService(AnimationService::getClassID()));

    std::map<std::string, Animation*>* animations = m_pAnimationService->GetAnimations();

    for (std::map<std::string, Animation*>::iterator i = animations->begin(); i != animations->end(); ++i)
    {
        QListWidgetItem *nodeListItem = new QListWidgetItem(animationList); 
        nodeListItem->setText(i->first.c_str()); 
    }
}

//--------------------------------------------------------------------------------------------------
void OsgControlWidget::ClearScene()
{
    animationList->reset();
    animationList->clear();
    QListWidgetItem* node = new QListWidgetItem(animationList);
    node->setText("[none]");
}
