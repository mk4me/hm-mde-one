#include "VideoPCH.h"
#include "VideoParser.h"
#include <plugins/video/Wrappers.h>
#include <vidlib/osg/VideoImageStream.h>
#include <vidlib/FileSequenceVideoStream.h>
#include <vidlib/FFmpegVideoStream.h>
#include <tinyxml.h>
#include <corelib/PluginCommon.h>
#include <corelib/ILog.h>
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
	adapter(core::ObjectWrapper::create< ::VideoStream>()),
	channelWrapper(core::ObjectWrapper::create<VideoChannel>())
{
}

VideoParser::~VideoParser()
{
}

void VideoParser::parse(const std::string & source)
{
	core::Filesystem::Path path(source);
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
            PLUGIN_LOG_INFO(directory << " " << framerate);


            // ustawienie ścieżki do katalogu
            core::Filesystem::Path dirPath = directory;
            if ( !dirPath.is_complete() ) {
                dirPath = path.branch_path() / directory;
            }
            // wylistowanie plików
            auto files = core::Filesystem::listFiles(dirPath.string());

            // tworzymy strumień z sekwencji plików
            osg::ref_ptr<osgDB::Options> options = new osgDB::Options();
            options->setObjectCacheHint( osgDB::Options::CACHE_NONE );
            std::auto_ptr<FileSequenceVideoStream> innerStream(new FileSequenceVideoStream(directory, framerate, files, new OsgImageLoader(options)));

            // UTILS_ASSERT(!innerStream->getLastError());

            // podpinay go pod adapter strumienia
            VideoStreamPtr realStream(new ::VideoStream(innerStream.release()));

            realStream->setTime(0);
            adapter->set(realStream);
			//metadane
            adapter->setMetadata("core/source", realStream->getSource());
        }
    } else {
        {
            // tworzymy strumień ffmpeg
            std::auto_ptr<FFmpegVideoStream> innerStream(new FFmpegVideoStream(path.string()));
            // UTILS_ASSERT(!innerStream->getLastError());

            // podpinay go pod adapter strumienia
            VideoStreamPtr realStream(new ::VideoStream(innerStream.release()));
            realStream->setTime(0);


            //adapter->set(realStream);
			//TODO

			VideoChannelPtr channel(new VideoChannel(realStream));
			channel->setName(path.filename().string());
			channelWrapper->set(channel);			
        }
    }
}

plugin::IParser* VideoParser::create() const
{
    return new VideoParser();
}

void VideoParser::acceptedExpressions(Expressions & expressions) const
{
	ExpressionDescription expDesc;
	expDesc.description = "Audio Video Interleaved format";
	expDesc.types.insert(typeid(VideoChannel));
	expressions.insert(Expressions::value_type(".*\\.avi$", expDesc));

	expDesc.description = "Moving Picture Experts Group format";
	expressions.insert(Expressions::value_type(".*\\.mpg$", expDesc));
	expressions.insert(Expressions::value_type(".*\\.mpeg$", expDesc));


	expDesc.description = "Custom image sequence format (XML based)";
	expDesc.types.clear();
	expDesc.types.insert(typeid(::VideoStream));
	expressions.insert(Expressions::value_type(".*\\.imgsequence$", expDesc));
}

void VideoParser::getObjects( core::Objects& objects )
{
    objects.insert(adapter);
	objects.insert(channelWrapper);
}
