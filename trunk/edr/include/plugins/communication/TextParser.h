#ifndef TEXTPARSER_H
#define TEXTPARSER_H

#include <core/IParser.h>

class TextParser : public core::IParser
{
    UNIQUE_ID('TXTP','PARS');
public:
    TextParser();

    virtual ~TextParser();

    virtual void parseFile(const std::string& path);

    virtual IParser* clone();

    virtual std::string getSupportedExtensions() const;

    virtual const std::string& getPath() const;

    const std::string& getOutput() const;
private:
    std::string extension;
    std::ifstream textFile;
    std::string path;
    std::string output;
};
#endif
