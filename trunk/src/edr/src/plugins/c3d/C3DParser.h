/********************************************************************
	created:	2011/05/25
	created:	25:5:2011   10:14
	filename: 	C3DParser.h
	author:		Wojciech Knieæ
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__C3DPARSER_H__
#define HEADER_GUARD_CORE__C3DPARSER_H__


#include <core/IParser.h>
#include <core/IDataManager.h>

class C3DParser : public core::IParser
{
    UNIQUE_ID("{D7801231-BACA-42C6-9A8E-706F561A563F}", "C3D parser");
private:
	std::vector<core::ObjectWrapperPtr> GRFChannels;
	std::vector<core::ObjectWrapperPtr> EMGChannels;
	core::ObjectWrapperPtr MarkerChannels;

public:
    C3DParser();
    virtual ~C3DParser();

public:
    virtual void parseFile(core::IDataManager* dataManager, const boost::filesystem::path& path);
    virtual core::IParser* create();
    virtual std::string getSupportedExtensions() const;
    virtual void getObjects(std::vector<core::ObjectWrapperPtr>& objects);
};


#endif