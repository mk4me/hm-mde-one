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
#include <boost/foreach.hpp>

//M_DECLARED_CLASS(VideoService, )


VideoService::VideoService()
:   name("Video")
{
    widget = new VideoWidget();
    optionsWidget = new VideoWidgetOptions(this);



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
	if(dataManager->getActualTrial().isVideos())
	{
		files = dataManager->getActualTrial().getVideosPaths();
	}
	//for(int i = 0; i < dataManager->getVideoFilePathCount(); i++)
	//{
 //       std::string path =  dataManager->getVideoFilePath(i);
 //       std::replace(path.begin(), path.end(), '/', '\\');
 //       files.push_back(path);
	//}

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
        // usuniêcie starych strumieni
        BOOST_FOREACH(timeline::StreamPtr stream, streams) {
            timeline->removeStream(stream);
        }
        streams.clear();

        // dodanie nowych strumieni
        BOOST_FOREACH(osg::Image* img, widget->getImages()) {
            if ( osg::ImageStream* stream = dynamic_cast<osg::ImageStream*>(img) ) {
                streams.push_back( timeline::StreamPtr(timeline::Stream::encapsulate(stream)) );
                timeline->addStream( streams.back() );
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
//     int idx = optionsWidget->outputFormatCombo->currentIndex();
//     if ( idx < utils::length(comboTranslationTable) ) {
//         return comboTranslationTable[idx];
//     } else {
    {
        UTILS_ASSERT(false, "Nieobs³ugiwany format.");
        return vm::PixelFormatUndefined;
    }
}


void VideoService::setOutputFormat( vm::PixelFormat format )
{
    UTILS_ASSERT(optionsWidget && widget);
//     if ( optionsWidget->outputFormatCombo->currentIndex() != i ) {
//         optionsWidget->blockSignals(true);
//         optionsWidget->outputFormatCombo->setCurrentIndex(i);
//         optionsWidget->blockSignals(false);
//     }
    widget->setPixelFormat(format);
}


bool VideoService::isUsingTextureRectangle()
{
    UTILS_ASSERT(optionsWidget);
    return optionsWidget->textureRectangleCheck->isChecked();
}

void VideoService::setUseTextureRectangle( bool useTextureRectangle )
{
    UTILS_ASSERT(optionsWidget && widget);
    QCheckBox* textureRectangleCheck = optionsWidget->textureRectangleCheck;
    if ( textureRectangleCheck->isChecked() ) {
        textureRectangleCheck->blockSignals(true);
        textureRectangleCheck->setChecked(useTextureRectangle);
        textureRectangleCheck->blockSignals(false);
    }
    optionsWidget->textureRectangleCheck->setChecked(useTextureRectangle);
    widget->setUseTextureRect(useTextureRectangle);
}

osg::Node* VideoService::debugGetLocalSceneRoot()
{
    return widget->getViewer()->getSceneData();
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