#include "VideoPCH.h"
#include "VideoParser.h"
#include <boost/filesystem.hpp>

using namespace vidlib;

VideoParser::VideoParser()
{
    object = core::ObjectWrapper::createWrapper<vidlib::VideoImageStream>();
}

VideoParser::~VideoParser()
{
}

void VideoParser::parseFile(const boost::filesystem::path& path)
{
    this->path = path;
    UTILS_ASSERT(stream == nullptr);
    osg::Image* image = osgDB::readImageFile(path.string());
    stream = dynamic_cast<VideoImageStream*>(image);
    if (!stream) {
        std::ostringstream buff;
        buff << "Error loading video file: " << path.string();
        throw new std::runtime_error(buff.str());
    } else {
        object->set<vidlib::VideoImageStream>(stream);
        object->setName(path.stem());
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

vidlib::VideoImageStream* VideoParser::getOsgStream()
{
    UTILS_ASSERT(stream);
    return stream.get();
}

vidlib::VideoStream* VideoParser::getStream()
{
    UTILS_ASSERT(stream);
    return nullptr;
    //return stream->getStream();
}

core::ObjectWrapperPtr VideoParser::getObject()
{
    return object;
}