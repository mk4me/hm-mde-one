
#include <iostream>
//#include <stack> // TODO: Wszystko std musi byæ w przysz³oœci na STLport (sugestia)

#include <osg/Node>
#include <osg/Group>
#include <QtCore/QTimeLine>

#include "TimeLine.h"
#include "ServiceManager.h"
//#include "../anim/AnimationService.h"


#define SLIDER_MAX 1000

//--------------------------------------------------------------------------------------------------
TimeLine::TimeLine(void):
  Ui::TimeLine()
, QWidget()
, _play(true)
, _animationTime(0.0f)
{
    setupUi(this); 
    QTimeLine *timeLine = new QTimeLine(1000, this);
    timeLine->setFrameRange(0, 100);
    timeLineSlider->setMaximum(SLIDER_MAX);

    connect(playButton, SIGNAL(pressed()), this, SLOT(PlayClicked()));
    connect(pauseButton, SIGNAL(pressed()), this, SLOT(PauseClicked()));
    connect(stopButton, SIGNAL(pressed()), this, SLOT(StopClicked()));

    connect(timeLineSlider, SIGNAL(sliderPressed()), this, SLOT(TimeLineSliderPressed()));
    //connect(timeLineSlider, SIGNAL(actionTriggered(int)), this, SLOT(timeLineSliderActionTriggered(int)));
    connect(timeLineSlider, SIGNAL(sliderReleased()), this, SLOT(TimeLineSliderReleased()));
    connect(timeLineSlider, SIGNAL(sliderMoved(int)), this, SLOT(TimeLineSliderMoved(int)));

//     AnimationService* animServ = ServiceManager::GetInstance()->GetSystemServiceAs<AnimationService>(); 
//     animServ->RegisterFunction(this, &TimeLine::Update);
//     animServ->RegisterOnStopFunction(this, &TimeLine::OnAnimationStop);
}

//--------------------------------------------------------------------------------------------------
TimeLine::~TimeLine(void)
{
//	AnimationService* animServ = ServiceManager::GetInstance()->GetSystemServiceAs<AnimationService>(); 
//	animServ->UnregisterFunction(this, &TimeLine::Update);
	Clear(); 
}

//--------------------------------------------------------------------------------------------------
// play button clicked
void TimeLine::PlayClicked()
{
/*	ServiceManager::GetInstance()->RegisterServiceAs<ObjectService>(); 
	ObjectService* objects = ServiceManager::GetInstance()->GetSystemServiceAs<ObjectService>();

	Model* model = dynamic_cast<Model*>(objects->GetModel());
	if (model)
	{
		// using AnimationService is more elegant than taking it directly from list
		AnimationService* animServ = ServiceManager::GetInstance()->GetSystemServiceAs<AnimationService>();

		if (animServ->GetSelectedAnimationName().length())
		{
			Animation* animation = model->GetAnimation(animServ->GetSelectedAnimationName());
			if (animation)
				animation->Play(model);
		}
	}*/
}

//--------------------------------------------------------------------------------------------------
// pause button clicked
void TimeLine::PauseClicked()
{
// 	AnimationService* animServ = ServiceManager::GetInstance()->GetSystemServiceAs<AnimationService>();
// 
// 	Animation* anim = animServ->GetAnimation();
// 	if (anim)
// 	{
// 		anim->Pause();
// 	}
}

//--------------------------------------------------------------------------------------------------
// stop button clicked
void TimeLine::StopClicked()
{ 
// 	AnimationService* animServ = ServiceManager::GetInstance()->GetSystemServiceAs<AnimationService>();
// 
// 	Animation* anim = animServ->GetAnimation();
// 	if (anim)
// 	{
// 		anim->Stop();
// 	}
}

//--------------------------------------------------------------------------------------------------
// call on animation stop
void TimeLine::OnAnimationStop()
{
	// TODO:
	// w zasadzie moglbym zrobnic to samo powyzej, ale
	// ten event w zasadzie moze gdzies, kiedys sie przydac
	// zatem wole zaznaczyc jego obecnosc

	timeLineSlider->setSliderPosition(0);
}

//--------------------------------------------------------------------------------------------------
// slider moved
void TimeLine::TimeLineSliderMoved(int p)
{
	TimeLineSliderReleased();
}

//--------------------------------------------------------------------------------------------------
// slider pressed
void TimeLine::TimeLineSliderPressed()
{
	std::cout << "timeLineSliderPressed()" << std::endl; 
	SetPause(true); 
}

//--------------------------------------------------------------------------------------------------
// slider released
void TimeLine::TimeLineSliderReleased()
{
/*	double pos = (double)timeLineSlider->sliderPosition() / SLIDER_MAX;

	// get services
	AnimationService* animServ = ServiceManager::GetInstance()->GetSystemServiceAs<AnimationService>(); 

	ServiceManager::GetInstance()->RegisterServiceAs<ObjectService>(); 
	ObjectService* objects = ServiceManager::GetInstance()->GetSystemServiceAs<ObjectService>();

	Model* model = dynamic_cast<Model*>(objects->GetModel());
	if (model && animServ->GetSelectedAnimationName().length())
	{
		Animation* animation = model->GetAnimation(animServ->GetSelectedAnimationName());
		if (animation)
		{
			animation->SetModel(model);
			currentTime->setValue(animation->SetPogress(pos));
			std::cout << pos << std::endl; 
		}	
	}*/
}

//--------------------------------------------------------------------------------------------------
void TimeLine::AnalizeNode( osg::Node *node )
{
	osg::Group *group = node->asGroup(); 

	osg::AnimationPathCallback *apc = dynamic_cast<osg::AnimationPathCallback *>(node->getUpdateCallback());

	if(apc)
	{
		std::cout << "AnimationPathCallback found! Node: " << node->getName() << std::endl; 
		_animPathCallbacks.push_back(apc); 
	}

	if(group)
	{
		int childCount = group->getNumChildren(); 
		for (int i=0; i<childCount; ++i)
		{
			osg::Node *child = group->getChild(i); 
			AnalizeNode(child); 
		}
	}
}

//--------------------------------------------------------------------------------------------------
void TimeLine::Clear()
{
  _animPathCallbacks.clear(); 
  _play = true; 
}

//--------------------------------------------------------------------------------------------------
void TimeLine::TogglePlay()
{
  _play = !_play; 
  SetPause(!_play); 
}

//--------------------------------------------------------------------------------------------------
void TimeLine::SetPause(bool pause)
{
  _play = !pause; 

  int animPathCallbackCount = _animPathCallbacks.size(); 
  for (int i=0; i<animPathCallbackCount; ++i)
  {
    osg::AnimationPathCallback *apc = _animPathCallbacks[i]; 
    apc->setPause(!_play); 
  }
}

//--------------------------------------------------------------------------------------------------
void TimeLine::AnimationTimeTest()
{
	int animPathCallbackCount = _animPathCallbacks.size(); 
	for (int i=0; i<animPathCallbackCount; ++i)
	{
		osg::AnimationPathCallback *apc = _animPathCallbacks[i]; 
		double time = GetCurrentAnimationTime(apc);
		double timeOffset = apc->getTimeOffset(); 
		std::cout << "Node: " << apc->getName() << " time: " << time << " timeOffset: " << timeOffset << std::endl;
	}
}

//--------------------------------------------------------------------------------------------------
void TimeLine::Update(double dt)
{
// 	AnimationService* animServ = ServiceManager::GetInstance()->GetSystemServiceAs<AnimationService>(); 
// 	Animation* anim = animServ->GetAnimation();
// 
// 	if (anim)
// 	{
// 		int pos = (int)(SLIDER_MAX * anim->GetProgress());
// 		timeLineSlider->setSliderPosition(pos);
// 		currentTime->setValue(anim->GetTime());
// 	}
}

//--------------------------------------------------------------------------------------------------
double TimeLine::GetCurrentAnimationTime( osg::AnimationPathCallback *apc )
{
  osg::AnimationPath *ap = apc->getAnimationPath(); 
  double time = apc->getAnimationTime(); 

  switch(ap->getLoopMode())
  {
  case(osg::AnimationPath::SWING):
    {
      double modulated_time = (time - ap->getFirstTime())/(ap->getPeriod()*2.0);
      double fraction_part = modulated_time - floor(modulated_time);
      if (fraction_part>0.5) fraction_part = 1.0-fraction_part;

      time = ap->getFirstTime()+(fraction_part*2.0) * ap->getPeriod();
      break;
    }
  case(osg::AnimationPath::LOOP):
    {
      double modulated_time = (time - ap->getFirstTime())/ap->getPeriod();
      double fraction_part = modulated_time - floor(modulated_time);
      time = ap->getFirstTime()+fraction_part * ap->getPeriod();
      break;
    }
  case(osg::AnimationPath::NO_LOOPING):
    // no need to modulate the time.
    break;
  }
  return time;   
}

//--------------------------------------------------------------------------------------------------
void TimeLine::SetScene( osgViewer::Scene *scene )
{
  Clear(); 
  osg::Node *node = scene->getSceneData(); 
  AnalizeNode(node); 

  double time = 0.0f; 
  int animPathCallbackCount = _animPathCallbacks.size(); 
  for (int i=0; i<animPathCallbackCount; ++i)
  {
    osg::AnimationPathCallback *apc = _animPathCallbacks[i]; 
    osg::AnimationPath *ap = apc->getAnimationPath(); 
    time += ap->getLastTime() - ap->getFirstTime(); 
    std::cout << "Node: " << apc->getName() << " time: " << ap->getLastTime() - ap->getFirstTime() << std::endl; 
  }
  time /= animPathCallbackCount; 
  _animationTime = time; 
}

//--------------------------------------------------------------------------------------------------
void TimeLine::TimeLineSliderActionTriggered( int act )
{
	int sliderMax = timeLineSlider->maximum(); 
	int sliderMin = timeLineSlider->minimum(); 
	int val = timeLineSlider->value(); 

	double mod_time = double(val)/double(sliderMax - sliderMin); 

	std::cout << "timeLineSliderValueChanged() " << mod_time << std::endl; 
	SetPause(true); 

	int animPathCallbackCount = _animPathCallbacks.size(); 
	for (int i=0; i<animPathCallbackCount; ++i)
	{
		osg::AnimationPathCallback *apc = _animPathCallbacks[i]; 
		osg::AnimationPath *ap = apc->getAnimationPath(); 

		double new_time = ap->getFirstTime() + mod_time*ap->getPeriod(); 

		//apc->setAnimationTime(new_time); 
	}
}

//--------------------------------------------------------------------------------------------------
void TimeLine::Echo()
{
	std::cout << "echo?" << std::endl; 
}