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
    //! Wewn�trzny strumie�.
    osg::ref_ptr<vidlib::VideoImageStream> stream;
    //! �cie�ka.
    std::string path;
    //! Obiekt wewn�trzny.
    core::ObjectWrapperPtr object;

public:
    VideoParser();
    virtual ~VideoParser();

// IParser
public:
    virtual void parseFile(const std::string& path);
    virtual core::IParser* create();
    virtual std::string getSupportedExtensions() const;
    virtual core::ObjectWrapperPtr getObject();

// IVideoParser
public:
    virtual vidlib::VideoImageStream* getOsgStream();
    virtual vidlib::VideoStream* getStream();
};


#endif  // __HEADER_GUARD_VIDEO__VIDEOPARSER_H__