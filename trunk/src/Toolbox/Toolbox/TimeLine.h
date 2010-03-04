#pragma once
#include <QtGui/QWidget>
#include <osg/AnimationPath>
#include <osgViewer/Scene>

#include "UI\ui_TimeLine.h"

class TimeLine: public QWidget, Ui::TimeLine
{
  Q_OBJECT
private: 
  std::vector<osg::AnimationPathCallback *> _animPathCallbacks; 
  bool _play; 
  float _animationTime; 
public:
  TimeLine(void);
  ~TimeLine(void);

  void analizeNode( osg::Node *node ); 
  void clear(); 
  void togglePlay(); 
  void setPause(bool pause); 
  void animationTimeTest(); 
  void update(); 
  double getCurrentAnimationTime( osg::AnimationPathCallback *apc ); 
  void setScene( osgViewer::Scene *scene ); 
  void timeLineSliderPressed(); 
  void timeLineSliderActionTriggered( int act ); 

public slots: 
  virtual void echo();
};
