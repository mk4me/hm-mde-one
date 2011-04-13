#include "VideoPCH.h"
#include "VideoParser.h"
#include <boost/filesystem.hpp>
#include <plugins/video/Wrappers.h>
#include <vidlib/osg/VideoImageStream.h>
#include <vidlib/FileSequenceVideoStream.h>
#include <vidlib/FFmpegVideoStream.h>
#include <tinyxml.h>
#include <core/Log.h>
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
    adapter = core::ObjectWrapper::createWrapper<::VideoStream>();

    //core::ObjectWrapperConstPtr ptr(__deprecated_stream);
    //osg::ref_ptr<const vidlib::VideoImageStream> obj = ptr->get<vidlib::VideoImageStream>();
}

VideoParser::~VideoParser()
{
}

void VideoParser::parseFile(core::IDataManager* /*dataManager*/, const boost::filesystem::path& path)
{
    namespace fs = boost::filesystem;
    if ( path.extension() == ".imgsequence" ) {
        
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
            fs::path dirPath = directory;
            if ( !dirPath.is_complete() ) {
                dirPath = path.branch_path() / directory;
            }
            // wylistowanie plików
            std::vector<std::string> files;
            for ( fs::directory_iterator it(dirPath), last; it != last; ++it ) {
                files.push_back(it->string());
            }


            // tworzymy strumieñ z sekwencji plików
            osg::ref_ptr<osgDB::Options> options = new osgDB::Options();
            options->setObjectCacheHint( osgDB::Options::CACHE_NONE );
            std::auto_ptr<FileSequenceVideoStream> innerStream(new FileSequenceVideoStream(directory, framerate, files, new OsgImageLoader(options)));

            // UTILS_ASSERT(!innerStream->getLastError());

            // podpinay go pod adapter strumienia
            VideoStreamPtr realStream(new ::VideoStream(innerStream.release()));

            realStream->setTime(0);
            adapter->set<::VideoStream>(realStream);
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


            adapter->set<::VideoStream>(realStream);
            adapter->setName(path.filename());
            adapter->setSource(path.string());
        }
    }
}

core::IParser* VideoParser::create()
{
    return new VideoParser();
}

std::string VideoParser::getSupportedExtensions() const
{
    return "avi;mpg;mpeg;imgsequence";
}

void VideoParser::getObjects( std::vector<core::ObjectWrapperPtr>& objects )
{
    objects.push_back(adapter);
}