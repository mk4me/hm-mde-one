#include "VideoPCH.h"
#include "VideoParser.h"
#include <plugins/video/Wrappers.h>
#include <vidlib/osg/VideoImageStream.h>
#include <vidlib/FileSequenceVideoStream.h>
#include <vidlib/FFmpegVideoStream.h>
#include <tinyxml2.h>
#include <osgGA/Event>
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



VideoParser::VideoParser()
{

}

VideoParser::~VideoParser()
{
}

void VideoParser::parse(const IStreamPtr stream, const std::string & source)
{
	this->stream = stream;
	utils::ObjectsVector localData;

	localData.push_back(utils::ObjectWrapper::create<VideoChannel>());

	// tworzymy strumień ffmpeg
	std::auto_ptr<FFmpegVideoStream> innerStream(new FFmpegVideoStream(stream, source));
	// UTILS_ASSERT(!innerStream->getLastError());

	// podpinay go pod adapter strumienia
	VideoStreamPtr realStream(new ::VideoStream(innerStream.release()));
	realStream->setTime(0);

	//adapter->set(realStream);
	//TODO

	VideoChannelPtr channel(new VideoChannel(realStream));
//	channel->setName(path.filename().string());
	localData[0]->set(channel);

	data.swap(localData);
}

void VideoParser::parse(const std::string & source)
{
	utils::ObjectsVector localData;	

	core::Filesystem::Path path(source);
    if ( core::Filesystem::fileExtension(path).compare(".imgsequence") == 0 ) {

		localData.push_back(utils::ObjectWrapper::create<::VideoStream>());

        std::ostringstream errbuff;
        tinyxml2::XMLDocument document;
		if (!document.LoadFile(path.string().c_str())) {
            errbuff << "Unable to load file: " << path.string();
            throw core::runtime_error(errbuff.str());
        }
        tinyxml2::XMLHandle hDocument(&document);
        tinyxml2::XMLElement* seq = hDocument.FirstChildElement("ImageSequence").ToElement();
        if ( !seq ) {
            errbuff << "Missing ImageSequence node";
            throw core::runtime_error(errbuff.str());
        } else {
            std::string directory;
            double framerate;
			auto ptr = seq->Attribute("directory");
            if ( !ptr ) {
                errbuff << "Missing directory attribute";
                throw core::runtime_error(errbuff.str());
			} else {
				directory = std::string(ptr);
			}
            if ( seq->QueryDoubleAttribute("framerate", &framerate) != tinyxml2::XML_SUCCESS ) {
                errbuff << "Missing framerate attribute";
                throw core::runtime_error(errbuff.str());
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
			localData[0]->set(realStream);
        }
    } else {
		localData.push_back(utils::ObjectWrapper::create<VideoChannel>());

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
		localData[0]->set(channel);
    }

	path = source;
	data.swap(localData);
}

void VideoParser::getObject(core::Variant& object, const core::VariantsVector::size_type idx) const
{
	UTILS_ASSERT((idx == 0), "Parser index out of range");
	object.set(data[idx]);
	object.setMetadata("core/source", path);
}

void VideoParser::reset()
{
	std::string().swap(path);
	utils::ObjectsVector().swap(data);
}

plugin::IParser* VideoParser::create() const
{
    return new VideoParser();
}

void VideoParser::acceptedExpressions(Expressions & expressions) const
{
	ExpressionDescription expDesc;
	expDesc.description = "Audio Video Interleaved format";
	expDesc.objectsTypes.push_back(typeid(VideoChannel));
	expressions.insert(Expressions::value_type(".*\\.avi$", expDesc));

	expDesc.description = "Moving Picture Experts Group format";
	expressions.insert(Expressions::value_type(".*\\.mpeg$", expDesc));

	expDesc.description = "Custom image sequence format (XML based)";
	expDesc.objectsTypes.clear();
	expDesc.objectsTypes.push_back(typeid(::VideoStream));
	expressions.insert(Expressions::value_type(".*\\.imgsequence$", expDesc));
}
