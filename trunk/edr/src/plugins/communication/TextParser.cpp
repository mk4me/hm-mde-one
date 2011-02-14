#include "CommunicationPCH.h"
#include <plugins/communication/TextParser.h>

TextParser::TextParser()
{
    extension = "TXT";
}

TextParser::~TextParser()
{
    textFile.close();
}

void TextParser::parseFile(const std::string& path)
{
    this->path = path;
    textFile.open(path.c_str());
    if(textFile.is_open())
    {
        textFile >> output;
    }
}

core::IParser* TextParser::clone()
{
    return new TextParser();
}

std::string TextParser::getSupportedExtensions() const
{
    return extension;
}

const std::string& TextParser::getOutput() const
{
    return output;
}

const std::string& TextParser::getPath() const
{
    return path;
}
