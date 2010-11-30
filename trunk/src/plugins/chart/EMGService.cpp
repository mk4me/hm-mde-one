#include "ChartPCH.h"

#include "EMGService.h"
#include "ChartWidget.h"
#include "ChartViewer.h"
#include <core/Chart.h>


using namespace std;
using namespace osg;

//deprecated:
//M_DECLARED_CLASS(AnimationService, kCLASSID_AnimationService);

// TODO przenieœæ to do osobnego pliku!
class ChartStream : public timeline::Stream
{
private:
  //! Strumieñ wewnêtrzny.
  EMGService* service;

public:
  //! \param stream Strumieñ wewnêtrzny.
  ChartStream(EMGService* service) : service(service)
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
template<> inline Stream* Stream::encapsulate(EMGService* service)
{
  return new ChartStream(service);
}
////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////

EMGService::EMGService()
: name("EMG"),targetTime(0.0),
  
  length(0.0)
{
    widget = new ChartWidget(this);
for(int i=12;i<28;i++){
		widget->addChart(i);
	}
	length=widget->getLenght() ;
	
}


EMGService::~EMGService()
{
}

IWidget* EMGService::getWidget()
{
	//TODO klasa widgetu
    return reinterpret_cast<IWidget*>(widget);
}

AsyncResult EMGService::init( IServiceManager* serviceManager, osg::Node* sceneRoot )
{
 ITimelinePtr timeline = core::queryServices<ITimeline>(serviceManager);
    if ( timeline ) {
        timeline->addStream( timeline::StreamPtr(timeline::Stream::encapsulate(this)) );
    } else {
        OSG_WARN<<"ITimeline not found."<<std::endl;
    }
    return AsyncResult_Complete;
}

AsyncResult EMGService::compute()
{
  
    return AsyncResult_Complete;
}

AsyncResult EMGService::update( double time, double timeDelta )
{

widget->update(targetTime);
return AsyncResult_Complete;
}

AsyncResult EMGService::lateUpdate( double time, double timeDelta)
{
   
    return AsyncResult_Complete;
}



const std::string& EMGService::getName() const
{
    return name;
}
void EMGService::setTargetTime(double time){
targetTime=time;

}
double EMGService::getTargetTime(){
return targetTime;
}
double EMGService::getLength() const
{
    return length;
}

void EMGService::setLength( double length )
{

    this->length = length;
}

void EMGService::setWidget(ChartWidget* widget){
this->widget=widget;
}

