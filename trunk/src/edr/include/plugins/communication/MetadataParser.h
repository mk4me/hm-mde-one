#ifndef HEADER_GUARD_COMMUNICATION_METADATAPARSER_H__
#define HEADER_GUARD_COMMUNICATION_METADATAPARSER_H__

#include <core/IParser.h>
#include <plugins/communication/Entity.h>

CORE_DEFINE_WRAPPER(communication::MetaData::MetaData, utils::PtrPolicyBoost, utils::ClonePolicyNotImplemented);

class MetadataParser : public core::IParser
{
    UNIQUE_ID("{9D535CB5-40B9-46B0-8F58-12999E993B32}", "Metadata Parser");
public:
    MetadataParser();

    virtual ~MetadataParser();
    
    virtual void parseFile(core::IDataManager* dataManager, const core::Filesystem::Path& path);

    virtual IParser* create();

    virtual std::string getSupportedExtensions() const;

    const communication::MetaData::MetaData& getMetadata() const;

    virtual void getObjects(std::vector<core::ObjectWrapperPtr>& objects);

private:
    core::Filesystem::Path path;
    core::ObjectWrapperPtr object;
};

typedef core::shared_ptr<MetadataParser> MetadataParserPtr;
typedef core::shared_ptr<const MetadataParser> MetadataParserConstPtr;
typedef core::weak_ptr<MetadataParser> MetadataParserWeakPtr;

#endif //HEADER_GUARD_COMMUNICATION_METADATAPARSER_H__
