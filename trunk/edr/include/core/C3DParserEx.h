#ifndef HEADER_GUARD_CORE_C3DPARSER_H__
#define HEADER_GUARD_CORE_C3DPARSER_H__

#include <core/IParser.h>
#include <core/c3dParser.h>

CORE_DEFINE_WRAPPER(C3D_Data, utils::PtrPolicyBoost);

class C3DParser : public core::IParser
{
    UNIQUE_ID('C3DP','PARS');

private:
    boost::filesystem::path path;
    std::vector<core::ObjectWrapperPtr> GRFChannels;
    std::vector<core::ObjectWrapperPtr> EMGChannels;

public:

    C3DParser();

    virtual ~C3DParser();
    
    virtual void parseFile(const boost::filesystem::path& path);

    virtual core::IParser* create();

    virtual std::string getSupportedExtensions() const;

    virtual void getObjects(std::vector<core::ObjectWrapperPtr>& objects);

    UTILS_DEPRECATED(C3D_Data* getC3dData());


};
#endif
