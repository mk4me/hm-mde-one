#include "VideoPCH.h"
#include <boost/foreach.hpp>
#include <core/IServiceManager.h>
#include <core/IDataManager.h>
#include "VideoService.h"
#include "VideoWidget.h"
#include "VideoWidgetOptions.h"
#include <vidlib/osg/VideoImageStream.h>
#include <boost/foreach.hpp>
#include <core/ILog.h>
#include <plugins/video/Wrappers.h>

using namespace core;


VideoService::VideoService()
:   name("Video")
{

}

void VideoService::loadData(IServiceManager* serviceManager, core::IDataManager* dataManager)
{

}

vidlib::PixelFormat VideoService::getOutputFormat()
{
    return vidlib::PixelFormatUndefined;
}


void VideoService::setOutputFormat( vidlib::PixelFormat format )
{

}


bool VideoService::isUsingTextureRectangle()
{

    return false;
}

void VideoService::setUseTextureRectangle( bool useTextureRectangle )
{

}

osg::Node* VideoService::debugGetLocalSceneRoot()
{
    return nullptr;
}