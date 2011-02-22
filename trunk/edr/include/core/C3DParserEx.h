#ifndef HEADER_GUARD_CORE_C3DPARSER_H__
#define HEADER_GUARD_CORE_C3DPARSER_H__

#include <core/IParser.h>
#include <core/c3dParser.h>

CORE_DEFINE_WRAPPER(C3D_Data, utils::PtrPolicyBoost);

class C3DParser : public core::IParser
{
    UNIQUE_ID('C3DP','PARS');
public:

    C3DParser();

    virtual ~C3DParser();
    
    virtual void parseFile(const boost::filesystem::path& path);

    virtual core::IParser* create();

    virtual std::string getSupportedExtensions() const;

    virtual core::ObjectWrapperPtr getObject();

    C3D_Data* getC3dData();

private:
    boost::filesystem::path path;
    core::ObjectWrapperPtr object;
};
#endif
