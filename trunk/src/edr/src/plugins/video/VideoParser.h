/********************************************************************
	created:  2011/02/04
	created:  4:2:2011   15:01
	filename: VideoParser.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_VIDEO__VIDEOPARSER_H__
#define __HEADER_GUARD_VIDEO__VIDEOPARSER_H__

#include <core/IParser.h>
#include <core/IDataManager.h>

class VideoParser : public core::IParser
{
    UNIQUE_ID("{E81ED99E-6682-4FE7-9E52-0BBCABC8BCBC}","Video Parser");
private:
    //! Obiekt wewn�trzny.
    core::ObjectWrapperPtr adapter;
    
public:
    VideoParser();
    virtual ~VideoParser();
// IParser
public:
    virtual void parseFile(core::IDataManager* dataManager, const core::Filesystem::Path& path);
    virtual core::IParser* create();
    virtual std::string getSupportedExtensions() const;
    virtual void getObjects(std::vector<core::ObjectWrapperPtr>& objects);
};


#endif  // __HEADER_GUARD_VIDEO__VIDEOPARSER_H__