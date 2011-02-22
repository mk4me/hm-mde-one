#include "CommunicationPCH.h"
#include <plugins/communication/TextParser.h>

TextParser::TextParser()
{
    extension = "TXT";
    object = core::ObjectWrapper::createWrapper(new std::string());
}

TextParser::~TextParser()
{
    textFile.close();
}

void TextParser::parseFile(const boost::filesystem::path& path)
{
    this->path = path;
    textFile.open(path.string().c_str());
    if(textFile.is_open())
    {
        textFile >> *object->get<std::string>();
    }
}

core::IParser* TextParser::create()
{
    return new TextParser();
}

std::string TextParser::getSupportedExtensions() const
{
    return extension;
}

const std::string& TextParser::getOutput() const
{
    return *object->get<std::string>();
}

core::ObjectWrapperPtr TextParser::getObject()
{
    return object;
}