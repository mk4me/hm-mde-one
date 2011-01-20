#include "CommunicationPCH.h"
#include <plugins/communication/TextParser.h>

TextParser::TextParser()
{
    extensions.push_back(".txt");
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

core::IParser::Extensions TextParser::getSupportedExtensions() const
{
    return extensions;
}

const std::string& TextParser::getOutput() const
{
    return output;
}

const std::string& TextParser::getPath() const
{
    return path;
}
