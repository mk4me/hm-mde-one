#include <iostream>
//#include <stack> // TODO: Wszystko std musi byæ w przysz³oœci na STLport (sugestia)

#include <osg/Node>
#include <osg/Group>
#include <QTimeLine>

#include "TimeLine.h"

TimeLine::TimeLine(void):
  Ui::TimeLine()
, QWidget()
, play_(true)
, animationTime_(0.0f)
{
  setupUi(this); 

  QTimeLine *timeLine = new QTimeLine(1000, this);
  timeLine->setFrameRange(0, 100);
  
  connect(togglePlayButton, SIGNAL(pressed()), this, SLOT(togglePlay()));
  connect(toolButton, SIGNAL(pressed()), this, SLOT(animationTimeTest()));
  connect(timeLineSlider, SIGNAL(sliderPressed()), this, SLOT(timeLineSliderPressed()));
  connect(timeLineSlider, SIGNAL(actionTriggered(int)), this, SLOT(timeLineSliderActionTriggered(int)));
}

TimeLine::~TimeLine(void)
{
  clear(); 
}

void TimeLine::analizeNode( osg::Node *node )
{
  osg::Group *group = node->asGroup(); 

  osg::AnimationPathCallback *apc = dynamic_cast<osg::AnimationPathCallback *>(node->getUpdateCallback());
  if(apc)
  {
    std::cout << "AnimationPathCallback found! Node: " << node->getName() << std::endl; 
    animPathCallbacks_.push_back(apc); 
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
  animPathCallbacks_.clear(); 
  play_ = true; 
}

void TimeLine::togglePlay()
{
  play_ = !play_; 
  setPause(!play_); 
}

void TimeLine::setPause(bool pause)
{
  play_ = !pause; 

  int animPathCallbackCount = animPathCallbacks_.size(); 
  for (int i=0; i<animPathCallbackCount; ++i)
  {
    osg::AnimationPathCallback *apc = animPathCallbacks_[i]; 
    apc->setPause(!play_); 
  }
}

void TimeLine::animationTimeTest()
{
  int animPathCallbackCount = animPathCallbacks_.size(); 
  for (int i=0; i<animPathCallbackCount; ++i)
  {
    osg::AnimationPathCallback *apc = animPathCallbacks_[i]; 
    double time = getCurrentAnimationTime(apc);
    double timeOffset = apc->getTimeOffset(); 
    std::cout << "Node: " << apc->getName() << " time: " << time << " timeOffset: " << timeOffset << std::endl;
  }
}

void TimeLine::update()
{
  if(!play_) return; 

  if(animPathCallbacks_.size() > 0)
  {
    double time = getCurrentAnimationTime(animPathCallbacks_[0]); 
    currentTime->setValue(time); 
    osg::AnimationPath *ap = animPathCallbacks_[0]->getAnimationPath(); 
    double period = ap->getPeriod(); 
    double modulated_time = (time - ap->getFirstTime())/ap->getPeriod();
    
    int sliderMax = timeLineSlider->maximum(); 
    int sliderMin = timeLineSlider->minimum(); 

    int currentValue = sliderMin + (sliderMax-sliderMin)*modulated_time; 

    timeLineSlider->setValue(currentValue); 
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
  int animPathCallbackCount = animPathCallbacks_.size(); 
  for (int i=0; i<animPathCallbackCount; ++i)
  {
    osg::AnimationPathCallback *apc = animPathCallbacks_[i]; 
    osg::AnimationPath *ap = apc->getAnimationPath(); 
    time += ap->getLastTime() - ap->getFirstTime(); 
    //std::cout << "Node: " << apc->getName() << " time: " << ap->getLastTime() - ap->getFirstTime() << std::endl; 
  }
  time /= animPathCallbackCount; 
  animationTime_ = time; 
}

void TimeLine::timeLineSliderPressed()
{
   std::cout << "timeLineSliderPressed()" << std::endl; 
  setPause(true); 
}

void TimeLine::timeLineSliderActionTriggered( int act )
{
  int sliderMax = timeLineSlider->maximum(); 
  int sliderMin = timeLineSlider->minimum(); 
  int val = timeLineSlider->value(); 

  double mod_time = double(val)/double(sliderMax - sliderMin); 

  std::cout << "timeLineSliderValueChanged() " << mod_time << std::endl; 
  setPause(true); 

  int animPathCallbackCount = animPathCallbacks_.size(); 
  for (int i=0; i<animPathCallbackCount; ++i)
  {
    osg::AnimationPathCallback *apc = animPathCallbacks_[i]; 
    osg::AnimationPath *ap = apc->getAnimationPath(); 

    double new_time = ap->getFirstTime() + mod_time*ap->getPeriod(); 

    //apc->setAnimationTime(new_time); 
  }
}

void TimeLine::echo()
{
  std::cout << "echo?" << std::endl; 
}