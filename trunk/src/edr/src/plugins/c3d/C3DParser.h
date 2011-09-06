/********************************************************************
	created:	2011/05/25
	created:	25:5:2011   10:14
	filename: 	C3DParser.h
	author:		Wojciech Knieæ
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_PLUGINC3D__C3DPARSER_H__
#define HEADER_GUARD_PLUGINC3D__C3DPARSER_H__

#include <core/SmartPtr.h>
#include <core/Filesystem.h>
#include <core/IParser.h>
#include <core/IDataManager.h>
#include <c3dlib/C3DParser.h>

class C3DParser : public core::IParser
{
    UNIQUE_ID("{D7801231-BACA-42C6-9A8E-706F561A563F}", "C3D parser");
	typedef core::shared_ptr<c3dlib::C3DParser> ParserPtr;
private:
	std::vector<core::ObjectWrapperPtr> GRFChannels;
	std::vector<core::ObjectWrapperPtr> EMGChannels;
	core::ObjectWrapperPtr GRFs;
	core::ObjectWrapperPtr EMGs;
	core::ObjectWrapperPtr markerChannels;
	core::ObjectWrapperPtr forceChannels;
	core::ObjectWrapperPtr angleChannels;
	core::ObjectWrapperPtr momentChannels;
	core::ObjectWrapperPtr powerChannels;
	core::ObjectWrapperPtr leftEvents;
	core::ObjectWrapperPtr rightEvents;
	//core::ObjectWrapperPtr c3dMisc;
	ParserPtr parserPtr;
	
public:
    C3DParser();
    virtual ~C3DParser();

public:
    virtual void parseFile(core::IDataManager* dataManager, const core::Filesystem::Path& path);
    virtual core::IParser* create();
    virtual std::string getSupportedExtensions() const;
    virtual void getObjects(std::vector<core::ObjectWrapperPtr>& objects);

	void saveFile(const core::Filesystem::Path& path);
};


#endif