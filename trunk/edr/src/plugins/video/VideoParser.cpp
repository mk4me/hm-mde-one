#include "VideoPCH.h"
#include "VideoParser.h"
#include <boost/filesystem.hpp>
#include <plugins/video/Wrappers.h>
#include <vidlib/osg/VideoImageStream.h>

using namespace vidlib;

VideoParser::VideoParser()
{
    __deprecated_stream = core::ObjectWrapper::createWrapper<vidlib::VideoImageStream>();
    stream = core::ObjectWrapper::createWrapper<::VideoStream>();

    //core::ObjectWrapperConstPtr ptr(__deprecated_stream);
    //osg::ref_ptr<const vidlib::VideoImageStream> obj = ptr->get<vidlib::VideoImageStream>();
}

VideoParser::~VideoParser()
{
}

void VideoParser::parseFile(core::IDataManager* /*dataManager*/, const boost::filesystem::path& path)
{
    UTILS_ASSERT(__deprecated_stream->isNull());
    // do za³adowania u¿ywamy systemu OSG
    osg::ref_ptr<osg::Image> image = osgDB::readImageFile(path.string());
    osg::ref_ptr<VideoImageStream> videoStream = osg::dynamic_pointer_cast<VideoImageStream>(image);
    if (!videoStream) {
        std::ostringstream buff;
        buff << "Error loading video file: " << path.string();
        throw new std::runtime_error(buff.str());
    } else {
        __deprecated_stream->set<vidlib::VideoImageStream>(videoStream);
        __deprecated_stream->setName(path.filename());
    }

    {
        VideoStreamPtr realStream(new ::VideoStream(path.string()));
        realStream->setTime(0);
        stream->set<::VideoStream>(realStream);
        stream->setName(realStream->getSource());
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
    objects.push_back(__deprecated_stream);
    objects.push_back(stream);
    objects.push_back(  core::ObjectWrapper::createWrapper<::VideoStream>() );
}