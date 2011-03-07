#ifndef HEADER_GUARD_COMMUNICATION_METADATAPARSER_H__
#define HEADER_GUARD_COMMUNICATION_METADATAPARSER_H__

#include <core/IParser.h>
#include <plugins/communication/MetaData.h>

CORE_DEFINE_WRAPPER(MetaData::MetaData, utils::PtrPolicyBoost);

class MetadataParser : public core::IParser
{
    UNIQUE_ID('MDDB','PARS');
public:
    MetadataParser();

    virtual ~MetadataParser();
    
    virtual void parseFile(const boost::filesystem::path& path);

    virtual IParser* create();

    virtual std::string getSupportedExtensions() const;

    const MetaData::MetaData& getMetadata() const;

    virtual void getObjects(std::vector<core::ObjectWrapperPtr>& objects);

private:
    boost::filesystem::path path;
    core::ObjectWrapperPtr object;
};

typedef core::shared_ptr<MetadataParser> MetadataParserPtr;
typedef core::shared_ptr<const MetadataParser> MetadataParserConstPtr;
typedef core::weak_ptr<MetadataParser> MetadataParserWeakPtr;

#endif
