#include "VideoPCH.h"
#include "VideoParser.h"
#include <boost/filesystem.hpp>
#include <plugins/video/Wrappers.h>
#include <vidlib/osg/VideoImageStream.h>

using namespace vidlib;

VideoParser::VideoParser()
{
    stream = core::ObjectWrapper::createWrapper<vidlib::VideoImageStream>();
}

VideoParser::~VideoParser()
{
}

void VideoParser::parseFile(const boost::filesystem::path& path)
{
    UTILS_ASSERT(stream->isNull());
    // do za³adowania u¿ywamy systemu OSG
    osg::ref_ptr<osg::Image> image = osgDB::readImageFile(path.string());
    osg::ref_ptr<VideoImageStream> videoStream = osg::dynamic_pointer_cast<VideoImageStream>(image);
    if (!videoStream) {
        std::ostringstream buff;
        buff << "Error loading video file: " << path.string();
        throw new std::runtime_error(buff.str());
    } else {
        stream->set<vidlib::VideoImageStream>(videoStream);
        stream->setName(path.filename());
    }
}

core::IParser* VideoParser::create()
{
    return new VideoParser();
}

std::string VideoParser::getSupportedExtensions() const
{
    return "avi;mpg;mpeg";
}

void VideoParser::getObjects( std::vector<core::ObjectWrapperPtr>& objects )
{
    objects.push_back(stream);
}