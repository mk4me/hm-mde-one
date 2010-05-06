#include <iostream>
//#include <stack> // TODO: Wszystko std musi byæ w przysz³oœci na STLport (sugestia)

#include <osg/Node>
#include <osg/Group>
#include <QTimeLine>

#include "TimeLine.h"
#include "ServiceManager.h"
#include "ObjectService.h"
#include "AnimationService.h"
#include "ModelWithSkeleton.h"


#define SLIDER_MAX 1000


TimeLine::~TimeLine(void)
{
	ServiceManager::GetInstance()->RegisterServiceAs<AnimationService>(); 
	AnimationService* animServ = ServiceManager::GetInstance()->GetSystemServiceAs<AnimationService>(); 
	animServ->unregisterFunction(this, &TimeLine::update);

	clear(); 
}


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

//	connect(togglePlayButton,	SIGNAL(pressed()),				this, SLOT(togglePlay()));
//	connect(toolButton,			SIGNAL(pressed()),				this, SLOT(animationTimeTest()));

	connect(playButton,			SIGNAL(pressed()),				this, SLOT(playClicked()));
	connect(pauseButton,		SIGNAL(pressed()),				this, SLOT(pauseClicked()));
	connect(stopButton,			SIGNAL(pressed()),				this, SLOT(stopClicked()));

	connect(timeLineSlider,		SIGNAL(sliderPressed()),		this, SLOT(timeLineSliderPressed()));
	// connect(timeLineSlider,		SIGNAL(actionTriggered(int)),	this, SLOT(timeLineSliderActionTriggered(int)));
	connect(timeLineSlider,		SIGNAL(sliderReleased()),		this, SLOT(timeLineSliderReleased()));
	connect(timeLineSlider,		SIGNAL(sliderMoved(int)),		this, SLOT(timeLineSliderMoved(int)));

	ServiceManager::GetInstance()->RegisterServiceAs<AnimationService>(); 
	AnimationService* animServ = ServiceManager::GetInstance()->GetSystemServiceAs<AnimationService>(); 
	animServ->registerFunction(this, &TimeLine::update);
	animServ->registerOnStopFunction(this, &TimeLine::onAnimationStop);
}


// play button clicked
void TimeLine::playClicked()
{
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
}


// pause button clicked
void TimeLine::pauseClicked()
{
	ServiceManager::GetInstance()->RegisterServiceAs<AnimationService>(); 
	AnimationService* animServ = ServiceManager::GetInstance()->GetSystemServiceAs<AnimationService>();

	CAnimation* anim = animServ->getAnimation();
	if (anim)
	{
		anim->pause();
	}
}


// stop button clicked
void TimeLine::stopClicked()
{
	ServiceManager::GetInstance()->RegisterServiceAs<AnimationService>(); 
	AnimationService* animServ = ServiceManager::GetInstance()->GetSystemServiceAs<AnimationService>();

	CAnimation* anim = animServ->getAnimation();
	if (anim)
	{
		anim->stop();
	}
}


// call on animation stop
void TimeLine::onAnimationStop()
{
	// TODO:
	// w zasadzie moglbym zrobnic to samo powyzej, ale
	// ten event w zasadzie moze gdzies, kiedys sie przydac
	// zatem wole zaznaczyc jego obecnosc

	timeLineSlider->setSliderPosition(0);
}


// slider moved
void TimeLine::timeLineSliderMoved(int p)
{
	timeLineSliderReleased();
}

// slider pressed
void TimeLine::timeLineSliderPressed()
{
	std::cout << "timeLineSliderPressed()" << std::endl; 
	setPause(true); 
}

// slider released
void TimeLine::timeLineSliderReleased()
{
	double pos = (double)timeLineSlider->sliderPosition() / SLIDER_MAX;

	// get services
	ServiceManager::GetInstance()->RegisterServiceAs<AnimationService>(); 
	AnimationService* animServ = ServiceManager::GetInstance()->GetSystemServiceAs<AnimationService>(); 

	ServiceManager::GetInstance()->RegisterServiceAs<ObjectService>(); 
	ObjectService* objects = ServiceManager::GetInstance()->GetSystemServiceAs<ObjectService>();

	CModelWithSkeleton* model = dynamic_cast<CModelWithSkeleton*>(objects->getModel());
	if (model && animServ->getSelectedAnimationName().length())
	{
		CAnimation* animation = model->getAnimation(animServ->getSelectedAnimationName());
		if (animation)
		{
			animation->setModel(model);
			currentTime->setValue(animation->setPogress(pos));
			std::cout << pos << std::endl; 
		}	
	}
}

void TimeLine::analizeNode( osg::Node *node )
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
			analizeNode(child); 
		}
	}
}

void TimeLine::clear()
{
  _animPathCallbacks.clear(); 
  _play = true; 
}

void TimeLine::togglePlay()
{
  _play = !_play; 
  setPause(!_play); 
}

void TimeLine::setPause(bool pause)
{
  _play = !pause; 

  int animPathCallbackCount = _animPathCallbacks.size(); 
  for (int i=0; i<animPathCallbackCount; ++i)
  {
    osg::AnimationPathCallback *apc = _animPathCallbacks[i]; 
    apc->setPause(!_play); 
  }
}

void TimeLine::animationTimeTest()
{
	int animPathCallbackCount = _animPathCallbacks.size(); 
	for (int i=0; i<animPathCallbackCount; ++i)
	{
		osg::AnimationPathCallback *apc = _animPathCallbacks[i]; 
		double time = getCurrentAnimationTime(apc);
		double timeOffset = apc->getTimeOffset(); 
		std::cout << "Node: " << apc->getName() << " time: " << time << " timeOffset: " << timeOffset << std::endl;
	}
}

void TimeLine::update(double dt)
{
	ServiceManager::GetInstance()->RegisterServiceAs<AnimationService>(); 
	AnimationService* animServ = ServiceManager::GetInstance()->GetSystemServiceAs<AnimationService>(); 
	CAnimation* anim = animServ->getAnimation();

	if (anim)
	{
		int pos = (int)(SLIDER_MAX * anim->getProgress());
		timeLineSlider->setSliderPosition(pos);
		currentTime->setValue(anim->getTime());
	}
}

double TimeLine::getCurrentAnimationTime( osg::AnimationPathCallback *apc )
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

void TimeLine::setScene( osgViewer::Scene *scene )
{
  clear(); 
  osg::Node *node = scene->getSceneData(); 
  analizeNode(node); 

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


void TimeLine::timeLineSliderActionTriggered( int act )
{
	int sliderMax = timeLineSlider->maximum(); 
	int sliderMin = timeLineSlider->minimum(); 
	int val = timeLineSlider->value(); 

	double mod_time = double(val)/double(sliderMax - sliderMin); 

	std::cout << "timeLineSliderValueChanged() " << mod_time << std::endl; 
	setPause(true); 

	int animPathCallbackCount = _animPathCallbacks.size(); 
	for (int i=0; i<animPathCallbackCount; ++i)
	{
		osg::AnimationPathCallback *apc = _animPathCallbacks[i]; 
		osg::AnimationPath *ap = apc->getAnimationPath(); 

		double new_time = ap->getFirstTime() + mod_time*ap->getPeriod(); 

		//apc->setAnimationTime(new_time); 
	}
}

void TimeLine::echo()
{
	std::cout << "echo?" << std::endl; 
}