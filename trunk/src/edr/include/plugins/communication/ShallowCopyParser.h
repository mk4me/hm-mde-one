#ifndef HEADER_GUARD_COMMUNICATION_SHALLOWCOPYPARSER_H__
#define HEADER_GUARD_COMMUNICATION_SHALLOWCOPYPARSER_H__

#include <core/IParser.h>
#include <plugins/communication/Entity.h>
#include <core/IDataManager.h>

CORE_DEFINE_WRAPPER(communication::ShallowCopy::ShallowCopy, utils::PtrPolicyBoost, utils::ClonePolicyNotImplemented);

typedef core::shared_ptr<communication::ShallowCopy::ShallowCopy> ShallowCopyPtr;
typedef core::shared_ptr<const communication::ShallowCopy::ShallowCopy> ShallowCopyConstPtr;

class ShallowCopyParser : public core::IParser
{
    UNIQUE_ID("{C11AD1FE-B44F-4F6B-80EE-B51DCE1758E8}", "Shallow Copy Parser");
public:
    ShallowCopyParser();

    virtual ~ShallowCopyParser();
    
    virtual void parseFile(const core::Filesystem::Path& path);

    virtual IParser* create();

    virtual void getSupportedExtensions(core::IParser::Extensions & extensions) const;

    const ShallowCopyConstPtr& getShallowCopy() const;

    virtual void getObjects(core::Objects& objects);

private:
    core::Filesystem::Path path;
    core::ObjectWrapperPtr object;
    ShallowCopyPtr shallowCopy;
    ShallowCopyConstPtr constShallowCopy;
};

typedef core::shared_ptr<ShallowCopyParser> ShallowCopyParserPtr;
typedef core::shared_ptr<const ShallowCopyParser> ShallowCopyParserConstPtr;
typedef core::weak_ptr<ShallowCopyParser> ShallowCopyParserWeakPtr;

#endif //HEADER_GUARD_COMMUNICATION_SHALLOWCOPYPARSER_H__
