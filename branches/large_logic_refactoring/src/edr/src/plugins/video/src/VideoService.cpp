#include "VideoPCH.h"
#include "VideoService.h"

using namespace core;


VideoService::VideoService()
:   widget(nullptr), optionsWidget(nullptr), name("Video")
{

}

vidlib::VIDLIB_PixelFormat VideoService::getOutputFormat()
{
    return vidlib::PixelFormatUndefined;
}


void VideoService::setOutputFormat( vidlib::VIDLIB_PixelFormat format )
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
