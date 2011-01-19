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

bool TextParser::isExtensionSupported(const std::string& extension) const
{
	if(extension.compare(extensions[0]) == 0)
	{
		return true;
	}
	return false;
}
