#ifndef IPARSER_H
#define IPARSER_H

#include <core/BaseDataTypes.h>
#include <core/IIdentifiable.h>
#include <core/SmartPtr.h>

namespace core
{
	class IParser : public IIdentifiable
	{
	public:
		typedef std::vector<std::string> Extensions;

		virtual ~IParser() {}

		virtual void parseFile(const std::string& path) = 0;

		virtual IParser* clone() = 0;

		virtual const Extensions& getSupportedExtensions() const = 0;

		virtual bool isExtensionSupported(const std::string& extension) const = 0;
	};

	typedef CORE_SHARED_PTR(IParser) IParserPtr;
	typedef CORE_CONST_SHARED_PTR(IParser) IParserConstPtr;
	typedef CORE_WEAK_PTR(IParser) IParserWeakPtr;
}
#endif
