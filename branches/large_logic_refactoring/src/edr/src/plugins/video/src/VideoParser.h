/********************************************************************
	created:  2011/02/04
	created:  4:2:2011   15:01
	filename: VideoParser.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_VIDEO__VIDEOPARSER_H__
#define __HEADER_GUARD_VIDEO__VIDEOPARSER_H__

#include <corelib/IParser.h>

class VideoParser : public plugin::IParser
{
    UNIQUE_ID("{E81ED99E-6682-4FE7-9E52-0BBCABC8BCBC}","Video Parser");
private:
    //! Obiekt wewnętrzny.
    core::ObjectWrapperPtr adapter;
	core::ObjectWrapperPtr channelWrapper;
    
public:
    VideoParser();
    virtual ~VideoParser();
// IParser
public:
    virtual void parseFile(const core::Filesystem::Path& path);
    virtual plugin::IParser* create();
    virtual void getSupportedExtensions(Extensions & extensions) const;    
    virtual void getObjects(core::Objects& objects);
};


#endif  // __HEADER_GUARD_VIDEO__VIDEOPARSER_H__
