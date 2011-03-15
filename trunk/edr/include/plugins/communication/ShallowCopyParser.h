#ifndef HEADER_GUARD_COMMUNICATION_SHALLOWCOPYPARSER_H__
#define HEADER_GUARD_COMMUNICATION_SHALLOWCOPYPARSER_H__

#include <core/IParser.h>
#include <plugins/communication/ShallowCopy.h>

CORE_DEFINE_WRAPPER(ShallowCopy::ShallowCopy, utils::PtrPolicyBoost);

class ShallowCopyParser : public core::IParser
{
    UNIQUE_ID('SCDB','PARS');
public:
    ShallowCopyParser();

    virtual ~ShallowCopyParser();
    
    virtual void parseFile(const boost::filesystem::path& path);

    virtual IParser* create();

    virtual std::string getSupportedExtensions() const;

    const ShallowCopy::ShallowCopy& getShallowCopy() const;

    virtual void getObjects(std::vector<core::ObjectWrapperPtr>& objects);

private:
    boost::filesystem::path path;
    core::ObjectWrapperPtr object;
};

typedef core::shared_ptr<ShallowCopyParser> ShallowCopyParserPtr;
typedef core::shared_ptr<const ShallowCopyParser> ShallowCopyParserConstPtr;
typedef core::weak_ptr<ShallowCopyParser> ShallowCopyParserWeakPtr;

#endif //HEADER_GUARD_COMMUNICATION_SHALLOWCOPYPARSER_H__
