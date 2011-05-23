#include "VideoPCH.h"
#include <boost/foreach.hpp>
#include <core/IServiceManager.h>
#include <plugins/timeline/ITimeline.h>
#include <core/IModel.h>
#include <core/IDataManager.h>
#include "VideoService.h"
#include "VideoWidget.h"
#include "VideoWidgetOptions.h"
#include "TimelineImageStreamWrapper.h"
#include <vidlib/osg/VideoImageStream.h>
#include <boost/foreach.hpp>
#include <core/Log.h>

#include <core/C3D_Data.h>
#include <core/C3DParserEx.h>
#include <plugins/video/Wrappers.h>

//M_DECLARED_CLASS(VideoService, )

using namespace core;

/**
 * 
 */
class TimelineVideoStream : public timeline::Stream
{
private:
    //! Strumieñ wewnêtrzny.
    VideoStreamPtr stream;

public:
    //! \param stream Strumieñ wewnêtrzny.
    TimelineVideoStream(const VideoStreamPtr& stream) : stream(stream)
    {}
    //! \see Stream::setTime
    virtual void setTime(double time)
    {
        stream->setTime(time);
    }
    //! \see Stream::getTime
    virtual double getTime() const
    {
        return stream->getTime();
    }
    //! \see Stream::getLength
    virtual double getLength() const
    {
        return stream->getDuration();
    }
};



VideoService::VideoService()
:   name("Video")
{


//     widget = new VideoWidget();
//     optionsWidget = new VideoWidgetOptions(this);
// 
// 
// 
// 
// //     std::vector<std::string> files;
// //     if ( files.size() == 0 ) {
// //         files.push_back("s1.avi");
// //         files.push_back("s1.avi");
// //         files.push_back("s1.avi";)
// //         files.push_back("s1.avi");
// //     }
// //     reinterpret_cast<VideoWidget*>(widget)->init(files);
// 
//     std::vector<std::string> files;
//     files.push_back("trial1a.avi");
//     files.push_back("trial1b.avi");
//     files.push_back("trial1c.avi");
//     files.push_back("trial1d.avi");
//     //for (int i = 0; i < dataManager->GetVideoFilePathCount(); ++i) {
//     //    files.push_back(dataManager->GetVideoFilePath(i));
//     //}
//     //((VideoWidget*)(widget))->init(files);
// 
//     // powi¹zanie akcji z kontrolkami z widgetu z opcjami (docelowo ma wylecieæ)
//     QObject::connect( widget->actionTextureRectangle, SIGNAL(toggled(bool)), optionsWidget->textureRectangleCheck, SLOT(setChecked(bool)) );
//     QObject::connect( optionsWidget->textureRectangleCheck, SIGNAL(toggled(bool)), widget->actionTextureRectangle, SLOT(setChecked(bool)) );
//     QObject::connect( widget->actionFormatYUV, SIGNAL(toggled(bool)), optionsWidget->radioYUV, SLOT(setChecked(bool)) );
//     QObject::connect( optionsWidget->radioYUV, SIGNAL(toggled(bool)), widget->actionFormatYUV, SLOT(setChecked(bool)) );
//     QObject::connect( widget->actionFormatRGB, SIGNAL(toggled(bool)), optionsWidget->radioRGB, SLOT(setChecked(bool)) );
//     QObject::connect( optionsWidget->radioRGB, SIGNAL(toggled(bool)), widget->actionFormatRGB, SLOT(setChecked(bool)) );
//     QObject::connect( widget->actionFormatBGRA, SIGNAL(toggled(bool)), optionsWidget->radioBGRA, SLOT(setChecked(bool)) );
//     QObject::connect( optionsWidget->radioBGRA, SIGNAL(toggled(bool)), widget->actionFormatBGRA, SLOT(setChecked(bool)) );
// 
//     // domyœlne wartoœci
//     widget->actionTextureRectangle->setChecked(true);
//     widget->actionFormatYUV->setChecked(true);
}

void VideoService::loadData(IServiceManager* serviceManager, core::IDataManager* dataManager)
{
// 
// 
// 	VideoWidget* widget = reinterpret_cast<VideoWidget*>(this->widget);
// 
// // 	std::vector<std::string> files;
// // 	if(dataManager->getActualLocalTrial().isVideos())
// // 	{
// // 		files = dataManager->getActualLocalTrial().getVideosPaths();
// // 	}
// 	//for(int i = 0; i < dataManager->getVideoFilePathCount(); i++)
// 	//{
//  //       std::string path =  dataManager->getVideoFilePath(i);
//  //       std::replace(path.begin(), path.end(), '/', '\\');
//  //       files.push_back(path);
// 	//}
// 
// //     files.push_back("trial1a.avi");
// //     files.push_back("trial1b.avi");
// //     files.push_back("trial1c.avi");
// //     files.push_back("trial1d.avi");
// //     //for (int i = 0; i < dataManager->GetVideoFilePathCount(); ++i) {
// //     //    files.push_back(dataManager->GetVideoFilePath(i));
// //     //}
// //     widget->init(files);
// 
//     widget->loadShaders("resources/shaders/textureRect_yuv_to_rgb.frag", "resources/shaders/texture2D_yuv_to_rgb.frag");
//     widget->setYuvImageSizeName("texture_size");
//     widget->setYuvSamplerName("movie_texture");
// 
//     std::vector< osg::ref_ptr<vidlib::VideoImageStream> > images;
//     core::queryData<vidlib::VideoImageStream>(dataManager, images);
//     widget->init(images);

//     std::vector<IVideoParserPtr> parsers;
//     core::queryParsers<IVideoParser>(dataManager, parsers);
//     widget->init(parsers);

//     std::vector< osg::ref_ptr<vidlib::VideoImageStream> > objects;
//     core::queryData< vidlib::VideoImageStream >(dataManager, objects);


//     ObjectWrapper* myStream;
// 
//     auto __streams = core::queryData<vidlib::VideoImageStream>(dataManager);
//     for (size_t i = 0; i < __streams.size(); ++i) {
//         const vidlib::VideoImageStream* stream = &*__streams[i];
//         vidlib::VideoImageStream* cloned = osg::clone( stream, osg::CopyOp::DEEP_COPY_ALL );
//         
//         myStream = ObjectWrapper::createWrapper(cloned);
//     }
// 
//     osg::ref_ptr<vidlib::VideoImageStream> cloned = myStream->get<vidlib::VideoImageStream>();

	//widget->init(files);

	ITimelinePtr timeline = core::queryServices<ITimeline>(serviceManager);
	if ( timeline ) {
        // usuniêcie starych strumieni
        BOOST_FOREACH(timeline::StreamPtr stream, streams) {
            timeline->removeStream(stream);
        }
        streams.clear();

//         // dodanie nowych strumieni
//         BOOST_FOREACH(osg::Image* img, widget->getImages()) {
//             if ( osg::ImageStream* stream = dynamic_cast<osg::ImageStream*>(img) ) {
//                 streams.push_back( timeline::StreamPtr(timeline::Stream::encapsulate(stream)) );
//                 timeline->addStream( streams.back() );
//             }
//         }

        std::vector<VideoStreamPtr> videoStreams = core::queryDataPtr(dataManager);
        BOOST_FOREACH(const VideoStreamPtr& stream, videoStreams) {
            streams.push_back( timeline::StreamPtr(new TimelineVideoStream(stream)) );
            timeline->addStream( streams.back() );
        }

	} else {
		OSG_WARN<<"ITimeline not found."<<std::endl;
	}
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

vidlib::PixelFormat VideoService::getOutputFormat()
{
//     UTILS_ASSERT(optionsWidget);
// //     int idx = optionsWidget->outputFormatCombo->currentIndex();
// //     if ( idx < utils::length(comboTranslationTable) ) {
// //         return comboTranslationTable[idx];
// //     } else {
//     {
//         UTILS_ASSERT(false, "Nieobs³ugiwany format.");
//         
//     }
    return vidlib::PixelFormatUndefined;
}


void VideoService::setOutputFormat( vidlib::PixelFormat format )
{
//     UTILS_ASSERT(optionsWidget && widget);
// //     if ( optionsWidget->outputFormatCombo->currentIndex() != i ) {
// //         optionsWidget->blockSignals(true);
// //         optionsWidget->outputFormatCombo->setCurrentIndex(i);
// //         optionsWidget->blockSignals(false);
// //     }
//     widget->setPixelFormat(format);
}


bool VideoService::isUsingTextureRectangle()
{
//     UTILS_ASSERT(optionsWidget);
//     return optionsWidget->textureRectangleCheck->isChecked();
    return false;
}

void VideoService::setUseTextureRectangle( bool useTextureRectangle )
{
//     UTILS_ASSERT(optionsWidget && widget);
//     QCheckBox* textureRectangleCheck = optionsWidget->textureRectangleCheck;
//     if ( textureRectangleCheck->isChecked() ) {
//         textureRectangleCheck->blockSignals(true);
//         textureRectangleCheck->setChecked(useTextureRectangle);
//         textureRectangleCheck->blockSignals(false);
//     }
//     optionsWidget->textureRectangleCheck->setChecked(useTextureRectangle);
//     widget->setUseTextureRect(useTextureRectangle);
}

osg::Node* VideoService::debugGetLocalSceneRoot()
{
    //return widget->getViewer()->getSceneData();
    return nullptr;
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