#include "VideoPCH.h"
#include <boost/foreach.hpp>
#include <core/IServiceManager.h>
#include <plugins/video/VideoService.h>
#include <plugins/timeline/ITimeline.h>
#include <core/IModel.h>
#include <core/IDataManager.h>
#include "VideoWidget.h"
#include "VideoWidgetQt.h"
#include "VideoWidgetOptions.h"
#include "TimelineImageStreamWrapper.h"
#include "osg/VideoImageStream.h"

//M_DECLARED_CLASS(VideoService, )

static vm::PixelFormat comboTranslationTable[] = {
    vm::PixelFormatYV12,
    vm::PixelFormatRGB24,
    vm::PixelFormatBGRA
};


VideoService::VideoService()
:   name("Video")
{
    widget = new VideoWidget();
    optionsWidget = new VideoWidgetOptions();
    //int * dupa = new int[100000];

//     std::vector<std::string> files;
//     if ( files.size() == 0 ) {
//         files.push_back("s1.avi");
//         files.push_back("s1.avi");
//         files.push_back("s1.avi";)
//         files.push_back("s1.avi");
//     }
//     reinterpret_cast<VideoWidget*>(widget)->init(files);

    std::vector<std::string> files;
    files.push_back("trial1a.avi");
    files.push_back("trial1b.avi");
    files.push_back("trial1c.avi");
    files.push_back("trial1d.avi");
    //for (int i = 0; i < dataManager->GetVideoFilePathCount(); ++i) {
    //    files.push_back(dataManager->GetVideoFilePath(i));
    //}
    //((VideoWidget*)(widget))->init(files);

}

AsyncResult VideoService::loadData(IServiceManager* serviceManager, IDataManager* dataManager)
{
	VideoWidget* widget = reinterpret_cast<VideoWidget*>(this->widget);

	std::vector<std::string> files;
	//TODO: ladowanie sciezek do plikow video [Marek Daniluk 23.11.10]
	for(int i = 0; i < dataManager->getVideoFilePathCount(); i++)
	{
        std::string path =  dataManager->getVideoFilePath(i);
        std::replace(path.begin(), path.end(), '/', '\\');
        files.push_back(path);
	}

//     files.push_back("trial1a.avi");
//     files.push_back("trial1b.avi");
//     files.push_back("trial1c.avi");
//     files.push_back("trial1d.avi");
//     //for (int i = 0; i < dataManager->GetVideoFilePathCount(); ++i) {
//     //    files.push_back(dataManager->GetVideoFilePath(i));
//     //}
//     widget->init(files);
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

AsyncResult VideoService::update( double time, double timeDelta )
{
    VideoWidgetOptions* optionsWidget = reinterpret_cast<VideoWidgetOptions*>(this->optionsWidget);
    return AsyncResult_Complete;
}

vm::PixelFormat VideoService::getOutputFormat()
{
    UTILS_ASSERT(optionsWidget);
    VideoWidgetOptions* optionsWidget = reinterpret_cast<VideoWidgetOptions*>(this->optionsWidget);
    int idx = optionsWidget->outputFormatCombo->currentIndex();
    if ( idx < utils::length(comboTranslationTable) ) {
        return comboTranslationTable[idx];
    } else {
        UTILS_ASSERT(false, "Nieobs³ugiwany format.");
        return vm::PixelFormatUndefined;
    }
}


void VideoService::setOutputFormat( vm::PixelFormat format )
{
    // wyszukanie indeksu komba
    size_t i;
    for ( i = 0; i < utils::length(comboTranslationTable) && comboTranslationTable[i] != format; ++i );
    // ustawienie komba i wartoœci
    if ( i != utils::length(comboTranslationTable) ) {
        UTILS_ASSERT(optionsWidget);
        VideoWidgetOptions* optionsWidget = reinterpret_cast<VideoWidgetOptions*>(this->optionsWidget);
        optionsWidget->outputFormatCombo->setCurrentIndex(i);
        // ustawienie formatu strumieni
//         BOOST_FOREACH( osg::Image* image, imagesList ) {
//             if ( vmOSGPlugin::VideoImageStream* stream = dynamic_cast<vmOSGPlugin::VideoImageStream>(image) ) {
//                 stream->setTargetFormat(format);
//             }
//         }

    } else {
        UTILS_ASSERT(false, "Nieobs³ugiwany format.");
    }
}

bool VideoService::isUsingTextureRectangle()
{
    UTILS_ASSERT(optionsWidget);
    VideoWidgetOptions* optionsWidget = reinterpret_cast<VideoWidgetOptions*>(this->optionsWidget);
    return optionsWidget->textureRectangleCheck->isChecked();
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