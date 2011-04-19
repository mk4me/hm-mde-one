#ifndef HEADER_GUARD_COMMUNICATION_SHALLOWCOPYPARSER_H__
#define HEADER_GUARD_COMMUNICATION_SHALLOWCOPYPARSER_H__

#include <core/IParser.h>
#include <plugins/communication/Entity.h>
#include <core/IDataManager.h>

CORE_DEFINE_WRAPPER(communication::ShallowCopy::ShallowCopy, utils::PtrPolicyBoost);

class ShallowCopyParser : public core::IParser
{
    UNIQUE_ID('SCDB','PARS');
public:
    ShallowCopyParser();

    virtual ~ShallowCopyParser();
    
    virtual void parseFile(core::IDataManager* dataManager, const boost::filesystem::path& path);

    virtual IParser* create();

    virtual std::string getSupportedExtensions() const;

    const communication::ShallowCopy::ShallowCopy& getShallowCopy() const;

    virtual void getObjects(std::vector<core::ObjectWrapperPtr>& objects);

private:
    boost::filesystem::path path;
    core::ObjectWrapperPtr object;
};

typedef core::shared_ptr<ShallowCopyParser> ShallowCopyParserPtr;
typedef core::shared_ptr<const ShallowCopyParser> ShallowCopyParserConstPtr;
typedef core::weak_ptr<ShallowCopyParser> ShallowCopyParserWeakPtr;

#endif //HEADER_GUARD_COMMUNICATION_SHALLOWCOPYPARSER_H__
