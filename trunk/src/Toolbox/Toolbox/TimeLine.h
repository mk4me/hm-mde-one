#ifndef TIMELINE_H
#define TIMELINE_H

#include <QtGui/QWidget>
#include <osg/AnimationPath>
#include <osgViewer/Scene>

#include "ui_TimeLine.h"

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
    void update(double dt); 
    double getCurrentAnimationTime( osg::AnimationPathCallback *apc ); 
    void setScene( osgViewer::Scene *scene ); 
    void timeLineSliderActionTriggered( int act ); 

	void onAnimationStop();
	
public slots: 

	virtual void playClicked();
	virtual void pauseClicked();
	virtual void stopClicked();

    virtual void echo();
	virtual void timeLineSliderMoved(int);
    virtual void timeLineSliderPressed();
	virtual void timeLineSliderReleased();
};


#endif