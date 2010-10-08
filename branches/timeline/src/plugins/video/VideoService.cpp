#include "VideoPCH.h"
#include <core/IServiceManager.h>
#include <plugins/video/VideoService.h>
#include <core/IModel.h>
#include <core/IDataManager.h>
#include "VideoWidget.h"
#include "VideoWidgetQt.h"
#include "TimelineImageStreamWrapper.h"

//M_DECLARED_CLASS(VideoService, )

VideoService::VideoService()
{
    widget = new VideoWidget();

    std::vector<std::string> files;
    if ( files.size() == 0 ) {
        files.push_back("s21.avi");
        files.push_back("s1.avi");
        files.push_back("s1.avi");
        files.push_back("s1.avi");
    }
    reinterpret_cast<VideoWidget*>(widget)->init(files);
}

AsyncResult VideoService::OnTick( double delta )
{
  return AsyncResult_Complete;
}

AsyncResult VideoService::OnAdded( IServiceManager* serviceManager )
{
  return AsyncResult_Complete;
}

void VideoService::SetModel(IDataManager* dataManager )
{
    std::vector<std::string> files;
    for (int i = 0; i < dataManager->GetFilePathCount(); ++i) {
        files.push_back(dataManager->GetVideoFilePath(i));
    }


    //widget->init()
}

timeline::Model::Streams VideoService::getStreams()
{
    timeline::Model::Streams streams;
    VideoWidget* widget = reinterpret_cast<VideoWidget*>(this->widget);

    for ( size_t i = 0; i < widget->getImages().size(); ++i ) {
        osg::Image* img = widget->getImages()[i].get();
        osg::ImageStream* stream = dynamic_cast<osg::ImageStream*>(img);
        if ( stream ) {
            streams.push_back(timeline::Model::StreamPtr(timeline::Stream::encapsulate(stream)));
        }
    }

    return streams;
}
