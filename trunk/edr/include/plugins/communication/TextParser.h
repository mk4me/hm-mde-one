#ifndef TEXTPARSER_H
#define TEXTPARSER_H

#include <core/IParser.h>

CORE_DEFINE_WRAPPER(std::string, utils::PtrPolicyBoost);

class TextParser : public core::IParser
{
    UNIQUE_ID('TXTP','PARS');
public:
    TextParser();

    virtual ~TextParser();
    
    virtual void setPath(const boost::filesystem::path& path)
    {
        this->path = path;
    };

    virtual bool isParsed()
    {
        return parsed;
    };

    virtual void parse();

    virtual IParser* create();

    virtual std::string getSupportedExtensions() const;

    const std::string& getOutput() const;

    virtual core::ObjectWrapperPtr getObject();

private:
    std::string extension;
    std::ifstream textFile;
    boost::filesystem::path path;
    core::ObjectWrapperPtr object;
    bool parsed;
};
#endif
