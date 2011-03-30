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
    UNIQUE_ID('VIDE','PARS');
private:
    //! Obiekt wewnêtrzny.
    core::ObjectWrapperPtr __deprecated_stream;
    core::ObjectWrapperPtr stream;
public:
    VideoParser();
    virtual ~VideoParser();
// IParser
public:
    virtual void parseFile(core::IDataManager* dataManager, const boost::filesystem::path& path);
    virtual core::IParser* create();
    virtual std::string getSupportedExtensions() const;
    virtual void getObjects(std::vector<core::ObjectWrapperPtr>& objects);
};


#endif  // __HEADER_GUARD_VIDEO__VIDEOPARSER_H__