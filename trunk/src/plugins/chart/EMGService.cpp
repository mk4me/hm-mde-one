#include "ChartPCH.h"

#include <core/IC3DModel.h>
#include <core/IDataManager.h>
#include "EMGService.h"
#include "ChartWidget.h"
#include "ChartViewer.h"
#include <core/Chart.h>
#include "C3DChartData.h"
#include <core/c3dParser.h>
#include <core/C3D_Data.h>
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

AsyncResult EMGService::init(IServiceManager* serviceManager, IDataManager* dataManager, osg::Node* sceneRoot, osgViewer::CompositeViewer* viewer)
{
    widget->getViewer()->onInit(viewer);
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
AsyncResult EMGService::loadData(IServiceManager* serviceManager, IDataManager* dataManager )
{
std::string c3dpath = "";
//if(dataManager->getC3dFilePathCount() > 0)
//	{
//		c3dpath = dataManager->getC3dFilePath(0);
//		c3dParser* parser =  new c3dParser();
//		C3D_Data* c3d = parser->parseData(c3dpath);
//		for(int i=12;i<28;i++){
//		widget->addChart(new C3DChartData(c3d,i));
//	}
//	length=widget->getLenght() ;
//	}
if(dataManager->getActualTrial().isC3d())
{
	widget->clear();
	c3dpath = dataManager->getActualTrial().getC3dPath();
	c3dParser* parser =  new c3dParser();
	C3D_Data* c3d = parser->parseData(c3dpath);
	for(int i=12;i<28;i++){
		widget->addChart(new C3DChartData(c3d,i));
	}
	length=widget->getLenght() ;
}

return AsyncResult_Complete;
}

osg::Node* EMGService::debugGetLocalSceneRoot()
{
    return widget->getViewer()->getSceneData();
}

void EMGService::visibilityChanged( IWidget* widget, bool visible )
{
    if ( widget == reinterpret_cast<IWidget*>(this->widget) ) {
        this->widget->getViewer()->setRenderingEnabled(visible);
    }
}