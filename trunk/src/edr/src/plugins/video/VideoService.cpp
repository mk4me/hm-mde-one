#include "VideoPCH.h"
#include "VideoService.h"

using namespace core;


VideoService::VideoService()
:   name("Video")
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