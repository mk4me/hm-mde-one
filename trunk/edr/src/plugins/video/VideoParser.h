/********************************************************************
	created:  2011/02/04
	created:  4:2:2011   15:01
	filename: VideoParser.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_VIDEO__VIDEOPARSER_H__
#define __HEADER_GUARD_VIDEO__VIDEOPARSER_H__

#include <plugins/video/IVideoParser.h>

class VideoParser : public IVideoParser
{
    UNIQUE_ID('VIDE','PARS');
private:
    //! Wewnêtrzny strumieñ.
    osg::ref_ptr<vidlib::VideoImageStream> stream;
    //! Œcie¿ka.
    boost::filesystem::path path;
    //! Obiekt wewnêtrzny.
    core::ObjectWrapperPtr object;
    //! czy juz u¿yto metody parse
    bool parsed;

public:
    VideoParser();
    virtual ~VideoParser();

// IParser
public:
    virtual void parse();
    virtual core::IParser* create();
    virtual std::string getSupportedExtensions() const;
    virtual core::ObjectWrapperPtr getObject();

    virtual void setPath(const boost::filesystem::path& path)
    {
        this->path = path;
    };

    virtual bool isParsed()
    {
        return parsed;
    };

// IVideoParser
public:
    virtual vidlib::VideoImageStream* getOsgStream();
    virtual vidlib::VideoStream* getStream();
};


#endif  // __HEADER_GUARD_VIDEO__VIDEOPARSER_H__