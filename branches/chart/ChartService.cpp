
#include <core/IServiceManager.h>
#include <plugins/chart/ChartService.h>
#include "ChartWidget.h"

#include <iostream>
#include <osgViewer/Scene>
#include <osg/Group>
#include <osg/Geode>
#include <osg/NodeVisitor>
#include <osg/Vec3d>
#include <osg/Quat>

#include <core/IModel.h>
#include <core/IDataManager.h>
#include <core/IMesh.h>
#include <core/Vec3.h>
#include <core/ModelData.h>

#include <core/ISkeletonNode.h>
#include <core/IServiceManager.h>

#include <plugins/timeline/ITimeline.h>
#include <plugins/timeline/Stream.h>


using namespace std;
using namespace osg;

//deprecated:
//M_DECLARED_CLASS(AnimationService, kCLASSID_AnimationService);

// TODO przenieœæ to do osobnego pliku!
class ChartStream : public timeline::Stream
{
private:
  //! Strumieñ wewnêtrzny.
  ChartService* service;

public:
  //! \param stream Strumieñ wewnêtrzny.
  ChartStream(ChartService* service) : service(service)
  {}
  //! \see Stream::setTime
  virtual void setTime(double time)
  {
    service->setTargetTime(time);
  }
  //! \see Stream::getTime
  virtual double getTime() const
  {
    return service->getTargetTime();
  }
  virtual double getLength() const
  {
    return service->getLength();
  }
};

////////////////////////////////////////////////////////////////////////////////
namespace timeline {
////////////////////////////////////////////////////////////////////////////////
template<> inline Stream* Stream::encapsulate(ChartService* service)
{
  return new ChartStream(service);
}
////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////

ChartService::ChartService()
: name("Chart"),targetTime(0.0),
  
  length(0.0)
{
    widget = new ChartWidget(this);
	length=(double)widget->getViewer()->getChart()->getFrameNumber(0)/(double)widget->getViewer()->getChart()->getFPS(0) ;
}


ChartService::~ChartService()
{
}

IWidget* ChartService::getWidget()
{
	//TODO klasa widgetu
    return reinterpret_cast<IWidget*>(widget);
}

AsyncResult ChartService::init( IServiceManager* serviceManager, osg::Node* sceneRoot )
{
    ITimelinePtr timeline = serviceManager->queryServices<ITimeline>();
    if ( timeline ) {
        timeline->addStream( timeline::StreamPtr(timeline::Stream::encapsulate(this)) );
    } else {
        OSG_WARN<<"ITimeline not found."<<std::endl;
    }
    return AsyncResult_Complete;
}

AsyncResult ChartService::compute()
{
  
    return AsyncResult_Complete;
}

AsyncResult ChartService::update( double time, double timeDelta )
{

	widget->getViewer()->getChart()->updatePointer(targetTime);
    return AsyncResult_Complete;
}

AsyncResult ChartService::lateUpdate( double time, double timeDelta)
{
   
    return AsyncResult_Complete;
}



const std::string& ChartService::getName() const
{
    return name;
}
void ChartService::setTargetTime(double time){
targetTime=time;

}
double ChartService::getTargetTime(){
return targetTime;
}
double ChartService::getLength() const
{
    return length;
}

void ChartService::setLength( double length )
{

    this->length = length;
}