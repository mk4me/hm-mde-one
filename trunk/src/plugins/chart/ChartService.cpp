#include "ChartPCH.h"


#include "ChartWidget.h"
#include "ChartViewer.h"
#include <core/Chart.h>


using namespace std;
using namespace osg;

//deprecated:
//M_DECLARED_CLASS(AnimationService, kCLASSID_AnimationService);

// TODO przenie�� to do osobnego pliku!
class ChartStream : public timeline::Stream
{
private:
  //! Strumie� wewn�trzny.
  ChartService* service;

public:
  //! \param stream Strumie� wewn�trzny.
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
	length=(double)widget->getViewer()->getChart()->getFrameNumber()/(double)widget->getViewer()->getChart()->getFPS() ;
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
 ITimelinePtr timeline = core::queryServices<ITimeline>(serviceManager);
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