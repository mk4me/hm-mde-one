#include "VideoPCH.h"
#include <core/IServiceManager.h>
#include <plugins/video/VideoService.h>
#include <plugins/timeline/ITimeline.h>
#include <core/IModel.h>
#include <core/IDataManager.h>
#include "VideoWidget.h"
#include "VideoWidgetQt.h"
#include "TimelineImageStreamWrapper.h"

//M_DECLARED_CLASS(VideoService, )

VideoService::VideoService()
:   name("Video")
{
    widget = new VideoWidget();
    //int * dupa = new int[100000];

//     std::vector<std::string> files;
//     if ( files.size() == 0 ) {
//         files.push_back("s1.avi");
//         files.push_back("s1.avi");
//         files.push_back("s1.avi";)
//         files.push_back("s1.avi");
//     }
//     reinterpret_cast<VideoWidget*>(widget)->init(files);
}

AsyncResult VideoService::loadData(IServiceManager* serviceManager, IDataManager* dataManager)
{
    VideoWidget* widget = reinterpret_cast<VideoWidget*>(this->widget);

    std::vector<std::string> files;
    for (int i = 0; i < dataManager->GetVideoFilePathCount(); ++i) {
        files.push_back(dataManager->GetVideoFilePath(i));
    }
    widget->init(files);

    ITimelinePtr timeline = core::queryServices<ITimeline>(serviceManager);
    if ( timeline ) {
        for ( size_t i = 0; i < widget->getImages().size(); ++i ) {
            osg::Image* img = widget->getImages()[i].get();
            osg::ImageStream* stream = dynamic_cast<osg::ImageStream*>(img);
            if ( stream ) {
                timeline->addStream(timeline::StreamPtr(timeline::Stream::encapsulate(stream)));
            }
        }
    } else {
        OSG_WARN<<"ITimeline not found."<<std::endl;
    }

    return AsyncResult_Complete;
// 
// 
//     for ( size_t i = 0; i < widget->getImages().size(); ++i ) {
//         osg::Image* img = widget->getImages()[i].get();
//         osg::ImageStream* stream = dynamic_cast<osg::ImageStream*>(img);
//         if ( stream ) {
//             streams.push_back(timeline::StreamPtr(timeline::Stream::encapsulate(stream)));
//         }
//     }

    //widget->init()
}

// timeline::Streams VideoService::getStreams()
// {
//     timeline::Streams streams;
//     VideoWidget* widget = reinterpret_cast<VideoWidget*>(this->widget);
// 
//     for ( size_t i = 0; i < widget->getImages().size(); ++i ) {
//         osg::Image* img = widget->getImages()[i].get();
//         osg::ImageStream* stream = dynamic_cast<osg::ImageStream*>(img);
//         if ( stream ) {
//             streams.push_back(timeline::StreamPtr(timeline::Stream::encapsulate(stream)));
//         }
//     }
// 
//     return streams;
// }
