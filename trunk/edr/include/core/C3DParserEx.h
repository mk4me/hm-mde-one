#ifndef C3DPARSER_H
#define C3DPARSER_H

#include <core/IParser.h>
#include <core/c3dParser.h>

CORE_DEFINE_WRAPPER(C3D_Data, utils::PtrPolicyBoost);

class C3DParser : public core::IParser
{
    UNIQUE_ID('C3DP','PARS');
public:

    C3DParser();

    virtual ~C3DParser();
    
    virtual void setPath(const boost::filesystem::path& path)
    {
        this->path = path;
    };

    virtual bool isParsed()
    {
        return parsed;
    };

    virtual void parse();

    virtual core::IParser* create();

    virtual std::string getSupportedExtensions() const;

    virtual core::ObjectWrapperPtr getObject();

    C3D_Data* getC3dData();

private:
    bool parsed;
    boost::filesystem::path path;
    core::ObjectWrapperPtr object;
};
#endif
