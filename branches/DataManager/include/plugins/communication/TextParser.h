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

	virtual const Extensions& getSupportedExtensions() const {return extensions;};

	virtual bool isExtensionSupported(const std::string& extension) const;

	const std::string& getOutput() const {return output;};

	const std::string& getPath() const {return path;};
private:
	Extensions extensions;
	std::ifstream textFile;
	std::string path;
	std::string output;
};
#endif
