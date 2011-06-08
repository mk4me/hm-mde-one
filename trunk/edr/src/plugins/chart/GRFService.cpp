#include "ChartPCH.h"

#include <core/IC3DModel.h>
#include <core/IDataManager.h>
#include "GRFService.h"
#include "ChartWidget.h"
#include "ChartViewer.h"
#include <core/Chart.h>
#include "C3DChartData.h"
#include <boost/foreach.hpp>
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
    widget = new deprecated__ChartWidget(this);
	
	length=widget->getLenght() ;
	



}


GRFService::~GRFService()
{
}

QWidget* GRFService::getWidget(std::vector<QObject*>& actions)
{
    return widget;
}

void GRFService::init(core::IServiceManager* serviceManager, core::IDataManager* dataManager)
{
    //widget->getViewer()->onInit(viewer);
 ITimelinePtr timeline = core::queryServices<ITimeline>(serviceManager);
    if ( timeline ) {
        timeline->addStream( timeline::StreamPtr(timeline::Stream::encapsulate(this)) );
    } else {
        OSG_WARN<<"ITimeline not found."<<std::endl;
    }
}

void GRFService::update( double time, double timeDelta )
{
	widget->update(targetTime);
}

const std::string& GRFService::getName() const
{
    return name;
}

void GRFService::setTargetTime(double time)
{
    targetTime=time;
}

double GRFService::getTargetTime()
{
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

void GRFService::setWidget(deprecated__ChartWidget* widget)
{
    this->widget=widget;
}




void GRFService::loadData(core::IServiceManager* serviceManager, core::IDataManager* dataManager )
{
    widget->clear();


    std::vector<core::GRFChannelPtr> channels = queryDataPtr(dataManager);


    BOOST_FOREACH( auto channel, channels ) {
        widget->addChart(channel);
    }
    length=widget->getLenght();


//     core::shared_ptr<C3DParser> parser = core::queryParsers<C3DParser>(dataManager);
//     if(parser)
//     {
//         widget->clear();
//         using namespace core;
//         if ( C3D_Data* c3d = parser->getC3dData() ) {
//         for(int i=0;i<12;i++){
//             widget->addChart(GRFChannelPtr( new GRFChannel(*c3d, i) ));
//         }
//         length=widget->getLenght() ;
//         }
//     }
//if(dataManager->getCurrentLocalTrial().isC3d())
//{
//	widget->clear();
//	c3dpath = dataManager->getCurrentLocalTrial().getC3dPath();
//	c3dParser* parser =  new c3dParser();
//	C3D_Data* c3d = parser->parseData(c3dpath);
//	for(int i=0;i<12;i++){
//		widget->addChart(new C3DChartData(c3d,i));
//	}
//	length=widget->getLenght() ;
//}
}

osg::Node* GRFService::debugGetLocalSceneRoot()
{
    return widget->getViewer()->getSceneData();
}