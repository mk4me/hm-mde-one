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

        virtual ~IParser() {}
        
        virtual void parseFile(const std::string& path) = 0;

        virtual const std::string& getPath() const = 0;

        virtual IParser* clone() = 0;

        virtual const std::string& getSupportedExtensions() const = 0;
    };

    typedef CORE_SHARED_PTR(IParser) IParserPtr;
    typedef CORE_CONST_SHARED_PTR(IParser) IParserConstPtr;
    typedef CORE_WEAK_PTR(IParser) IParserWeakPtr;
}
#endif
