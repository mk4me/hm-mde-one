#ifndef HEADER_GUARD_COMMUNICATION_TEXTPARSER_H__
#define HEADER_GUARD_COMMUNICATION_TEXTPARSER_H__

#include <core/IParser.h>

CORE_DEFINE_WRAPPER(std::string, utils::PtrPolicyBoost);

class TextParser : public core::IParser
{
    UNIQUE_ID('TXTP','PARS');
public:
    TextParser();

    virtual ~TextParser();
    
    virtual void parseFile(const boost::filesystem::path& path);

    virtual IParser* create();

    virtual std::string getSupportedExtensions() const;

    const std::string& getOutput() const;

    virtual core::ObjectWrapperPtr getObject();

private:
    std::string extension;
    std::ifstream textFile;
    boost::filesystem::path path;
    core::ObjectWrapperPtr object;
};
#endif
