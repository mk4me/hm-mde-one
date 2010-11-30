#include "ChartPCH.h"

#include "GRFService.h"
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
  GRFService* service;

public:
  //! \param stream Strumieñ wewnêtrzny.
  ChartStream(GRFService* service) : service(service)
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
template<> inline Stream* Stream::encapsulate(GRFService* service)
{
  return new ChartStream(service);
}
////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////

GRFService::GRFService()
: name("GRF"),targetTime(0.0),
  
  length(0.0)
{
    widget = new ChartWidget(this);
	
	for(int i=0;i<12;i++){
		widget->addChart(i);
	}
	length=widget->getLenght() ;
	
}


GRFService::~GRFService()
{
}

IWidget* GRFService::getWidget()
{
	//TODO klasa widgetu
    return reinterpret_cast<IWidget*>(widget);
}

AsyncResult GRFService::init( IServiceManager* serviceManager, osg::Node* sceneRoot )
{
 ITimelinePtr timeline = core::queryServices<ITimeline>(serviceManager);
    if ( timeline ) {
        timeline->addStream( timeline::StreamPtr(timeline::Stream::encapsulate(this)) );
    } else {
        OSG_WARN<<"ITimeline not found."<<std::endl;
    }
    return AsyncResult_Complete;
}

AsyncResult GRFService::compute()
{
  
    return AsyncResult_Complete;
}

AsyncResult GRFService::update( double time, double timeDelta )
{

	widget->update(targetTime);
    return AsyncResult_Complete;
}

AsyncResult GRFService::lateUpdate( double time, double timeDelta)
{
   
    return AsyncResult_Complete;
}



const std::string& GRFService::getName() const
{
    return name;
}
void GRFService::setTargetTime(double time){
targetTime=time;

}
double GRFService::getTargetTime(){
return targetTime;
}
double GRFService::getLength() const
{
    return length;
}

void GRFService::setLength( double length )
{

    this->length = length;
}

void GRFService::setWidget(ChartWidget* widget){
this->widget=widget;
}

