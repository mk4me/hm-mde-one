#include "VideoPCH.h"
#include "VideoParser.h"
#include <plugins/video/Wrappers.h>
#include <vidlib/osg/VideoImageStream.h>
#include <vidlib/FileSequenceVideoStream.h>
#include <vidlib/FFmpegVideoStream.h>
#include <tinyxml.h>
#include <core/PluginCommon.h>
#include <core/ILog.h>
#include <osg/ImageSequence>
#include <osgDB/ReadFile>

using namespace vidlib;

class OsgImageLoader : public FileSequenceVideoStream::ILoader
{
    osg::ref_ptr<osg::Image> img;
    osg::ref_ptr<osgDB::Options> options;

public:
    OsgImageLoader(osgDB::Options* options) 
        : options(options)
    {
    }

    bool readImage(const std::string& path, Picture& picture)
    {
        img = osgDB::readImageFile(path, options.get());
        if ( img ) {
            if (img->getOrigin() == osg::Image::BOTTOM_LEFT) {
                img->flipVertical();
                img->setOrigin(osg::Image::TOP_LEFT);
            }
            if ( img->getPixelFormat() == GL_RGB ) {
                picture.format = vidlib::PixelFormatRGB24;
                picture.width = img->s();
                picture.height = img->t();
                picture.dataHeight = picture.height;
                picture.dataWidth = 3 * picture.width;
                picture.data = (unsigned char*)(img->getDataPointer());
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }

    }

    ILoader* clone() const
    {
        return new OsgImageLoader(options);
    }
};



VideoParser::VideoParser() : 
	adapter(core::ObjectWrapper::create<::VideoStream>()),
	channelWrapper(core::ObjectWrapper::create<VideoChannel>())
{
}

VideoParser::~VideoParser()
{
}

void VideoParser::parseFile(const core::Filesystem::Path& path)
{
    if ( core::Filesystem::fileExtension(path).compare(".imgsequence") == 0 ) {
        
        std::ostringstream errbuff;
        TiXmlDocument document(path.string());
        if ( !document.LoadFile() ) {
            errbuff << document.ErrorDesc();
            throw std::runtime_error(errbuff.str());
        }
        TiXmlHandle hDocument(&document);
        TiXmlElement* seq = hDocument.FirstChild("ImageSequence").ToElement();
        if ( !seq ) {
            errbuff << "Missing ImageSequence node";
            throw std::runtime_error(errbuff.str());
        } else {
            std::string directory;
            double framerate;
            if ( seq->QueryStringAttribute("directory", &directory) != TIXML_SUCCESS ) {
                errbuff << "Missing directory attribute";
                throw std::runtime_error(errbuff.str());
            }
            if ( seq->QueryDoubleAttribute("framerate", &framerate) != TIXML_SUCCESS ) {
                errbuff << "Missing framerate attribute";
                throw std::runtime_error(errbuff.str());
            }
            LOG_INFO(directory << " " << framerate);

            
            // ustawienie œcie¿ki do katalogu
            core::Filesystem::Path dirPath = directory;
            if ( !dirPath.is_complete() ) {
                dirPath = path.branch_path() / directory;
            }
            // wylistowanie plików
            std::vector<std::string> files = core::Filesystem::listFiles(dirPath);

            // tworzymy strumieñ z sekwencji plików
            osg::ref_ptr<osgDB::Options> options = new osgDB::Options();
            options->setObjectCacheHint( osgDB::Options::CACHE_NONE );
            std::auto_ptr<FileSequenceVideoStream> innerStream(new FileSequenceVideoStream(directory, framerate, files, new OsgImageLoader(options)));

            // UTILS_ASSERT(!innerStream->getLastError());

            // podpinay go pod adapter strumienia
            VideoStreamPtr realStream(new ::VideoStream(innerStream.release()));

            realStream->setTime(0);
            adapter->set(realStream);
            adapter->setName(realStream->getSource());
            adapter->setSource(directory);
        }
    } else {
        {
            // tworzymy strumieñ ffmpeg
            std::auto_ptr<FFmpegVideoStream> innerStream(new FFmpegVideoStream(path.string()));
            // UTILS_ASSERT(!innerStream->getLastError());

            // podpinay go pod adapter strumienia
            VideoStreamPtr realStream(new ::VideoStream(innerStream.release()));
            realStream->setTime(0);


            adapter->set(realStream);
            adapter->setName(path.filename().string());
            adapter->setSource(path.string());

			VideoChannelPtr channel(new VideoChannel(realStream));
			channel->setName(path.filename().string());
			channelWrapper->set(channel);
			channelWrapper->setName(path.filename().string());
			channelWrapper->setSource(path.string());
        }
    }
}

core::IParser* VideoParser::create()
{
    return new VideoParser();
}

void VideoParser::getSupportedExtensions(Extensions & extensions) const
{
    core::IParser::ExtensionDescription extDesc;
    extDesc.description = "Audio Video Interleaved format";

    extDesc.types.insert(typeid(VideoChannel));

    extensions["avi"] = extDesc;

    extDesc.description = "Moving Picture Experts Group format";

    extensions["mpg"] = extDesc;

    extensions["mpeg"] = extDesc;

    extDesc.description = "Custom image sequence format (XML based)";

    extDesc.types.clear();
    extDesc.types.insert(typeid(::VideoStream));

    extensions["imgsequence"] = extDesc;
}

void VideoParser::getObjects( core::Objects& objects )
{
    objects.insert(adapter);
	objects.insert(channelWrapper);
}