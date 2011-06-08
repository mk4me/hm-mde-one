#include "ChartPCH.h"

#include <core/IC3DModel.h>
#include <core/IDataManager.h>
#include "EMGService.h"
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
    widget = new deprecated__ChartWidget(this);

	length=widget->getLenght() ;
	
}

EMGService::~EMGService()
{
}

QWidget* EMGService::getWidget(std::vector<QObject*>& actions)
{
    return widget;
}

void EMGService::init(core::IServiceManager* serviceManager, core::IDataManager* dataManager)
{
    //widget->getViewer()->onInit(viewer);
    ITimelinePtr timeline = core::queryServices<ITimeline>(serviceManager);
    if ( timeline ) {
        timeline->addStream( timeline::StreamPtr(timeline::Stream::encapsulate(this)) );
    } else {
        OSG_WARN<<"ITimeline not found."<<std::endl;
    }
}

void EMGService::update( double time, double timeDelta )
{
    widget->update(targetTime);
}

const std::string& EMGService::getName() const
{
    return name;
}
void EMGService::setTargetTime(double time)
{
    targetTime=time;
}

double EMGService::getTargetTime()
{
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

void EMGService::setWidget(deprecated__ChartWidget* widget)
{
    this->widget=widget;
}

void EMGService::loadData(core::IServiceManager* serviceManager, core::IDataManager* dataManager )
{
    widget->clear();
    auto channels = core::queryDataPtr<core::EMGChannelPtr>(dataManager);
    BOOST_FOREACH( auto channel, channels ) {
        widget->addChart(channel);
    }
    length=widget->getLenght() ;


//     core::shared_ptr<C3DParser> parser = core::queryParsers<C3DParser>(dataManager);
//     if(parser)
//     {
//         widget->clear();
//         using namespace core;
// //         for (int j = 0; j < 1; ++j) {
// //             EMGChannelPtr channel = EMGChannelPtr(new EMGChannel(33));
// //             for ( int i = 0; i < 500; ++i ) {
// //                 channel->addPoint(i /*% 2 ? 100 : 0*/);
// //             }
// //             channel->normalize();
// //             widget->addChart(channel);
// // 
// //             channel = EMGChannelPtr(new EMGChannel(33));
// //             for ( int i = 0; i < 500; ++i ) {
// //                 channel->addPoint(i % 2 ? 100 : 0);
// //             }
// //             channel->normalize();
// //             widget->addChart(channel);
// // 
// //             channel = EMGChannelPtr(new EMGChannel(33));
// //             for ( int i = 0; i < 500; ++i ) {
// //                 channel->addPoint( rand() % 100 );
// //             }
// //             channel->normalize();
// //             widget->addChart(channel);
// //         }
// 
//         
//         C3D_Data* c3d = parser->getC3dData();
//         for(int i=12;i<28;i++){
//             widget->addChart(EMGChannelPtr( new EMGChannel(*c3d, i) ));
//         }
//         length=widget->getLenght() ;
//     }
//if(dataManager->getCurrentLocalTrial().isC3d())
//{
//	widget->clear();
//	c3dpath = dataManager->getCurrentLocalTrial().getC3dPath();
//	c3dParser* parser =  new c3dParser();
//	C3D_Data* c3d = parser->parseData(c3dpath);
//	for(int i=12;i<28;i++){
//		widget->addChart(new C3DChartData(c3d,i));
//	}
//	length=widget->getLenght() ;
//}

}

osg::Node* EMGService::debugGetLocalSceneRoot()
{
    return widget->getViewer()->getSceneData();
}

////////////////////////////////////////////////////////////////////////////////

class ScalarChannelStream : public timeline::Stream
{
private:
    //! Strumieñ wewnêtrzny.
    core::C3DAnalogChannelPtr channel;

public:
    //! \param stream Strumieñ wewnêtrzny.
    ScalarChannelStream(const core::C3DAnalogChannelPtr& channel) : channel(channel)
    {}
    //! \see Stream::setTime
    virtual void setTime(double time)
    {
        channel->setTime(time);
    }
    //! \see Stream::getTime
    virtual double getTime() const
    {
        return channel->getTime();
    }
    virtual double getLength() const
    {
        return channel->getLength();
    }
};

ChartService::ChartService() : name("ChartService")
{
    using namespace core;
//     ObjectWrapperPtr wrapp = ObjectWrapper::create<EMGChannel>();
//     wrapp->set( EMGChannelPtr(new EMGChannel(10)) );
// 
//     EMGChannelPtr ptr;
//     EMGChannelConstPtr constPtr;
//     wrapp->tryGet2(ptr);
//     wrapp->tryGet2(constPtr);
// 
// 
//     ScalarChannelPtr ptr2;
//     ScalarChannelConstPtr constPtr2;
//     wrapp->tryGet2(ptr2);
//     wrapp->tryGet2(constPtr2);
// 
//     ptr2.reset();
//     constPtr2.reset();
//     wrapp->tryGet2(ptr2, true);
//     wrapp->tryGet2(constPtr2, true);
// 
//     GRFChannelPtr ptr3;
//     GRFChannelConstPtr constPtr3;
//     wrapp->tryGet2(ptr3);
//     wrapp->tryGet2(constPtr3);

    int brrr = 0;
}

void ChartService::loadData( core::IServiceManager* serviceManager, core::IDataManager* dataManager )
{
    ITimelinePtr timelinesrv = core::queryServices<ITimeline>(serviceManager);
    if ( timelinesrv ) {
        // usuniêcie starych strumieni
        BOOST_FOREACH(timeline::StreamPtr stream, streams) {
            timelinesrv->removeStream(stream);
        }
        streams.clear();
        // dodanie nowych
        std::vector<core::C3DAnalogChannelPtr> videoStreams = core::queryDataPtr(dataManager);
        BOOST_FOREACH(const core::C3DAnalogChannelPtr& stream, videoStreams) {
            streams.push_back( timeline::StreamPtr(new ScalarChannelStream(stream)) );
            timelinesrv->addStream( streams.back() );
        }
    }
}

QWidget* ChartService::getWidget(std::vector<QObject*>& actions)
{
    return nullptr;
}

const std::string& ChartService::getName() const
{
    return name;
}