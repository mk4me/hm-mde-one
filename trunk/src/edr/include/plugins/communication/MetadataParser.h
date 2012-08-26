#ifndef HEADER_GUARD_COMMUNICATION_METADATAPARSER_H__
#define HEADER_GUARD_COMMUNICATION_METADATAPARSER_H__

#include <core/IParser.h>
#include <plugins/communication/Entity.h>

CORE_DEFINE_WRAPPER(communication::MotionMetaData::MetaData, utils::PtrPolicyBoost, utils::ClonePolicyNotImplemented);

class MotionMetadataParser : public core::IParser
{
    UNIQUE_ID("{9D535CB5-40B9-46B0-8F58-12999E993B32}", "Motion Metadata Parser");
public:
    MotionMetadataParser();

    virtual ~MotionMetadataParser();
    
    virtual void parseFile(const core::Filesystem::Path& path);

    virtual IParser* create();

    virtual void getSupportedExtensions(core::IParser::Extensions &) const;

    const communication::MotionMetaData::MetaData& getMetadata() const;

    virtual void getObjects(core::Objects& objects);

private:
    core::Filesystem::Path path;
    core::ObjectWrapperPtr object;
    core::shared_ptr<communication::MotionMetaData::MetaData> metadata;
};

typedef core::shared_ptr<MotionMetadataParser> MotionMetadataParserPtr;
typedef core::shared_ptr<const MotionMetadataParser> MotionMetadataParserConstPtr;
typedef core::weak_ptr<MotionMetadataParser> MotionMetadataParserWeakPtr;

CORE_DEFINE_WRAPPER(communication::MedicalMetaData::MetaData, utils::PtrPolicyBoost, utils::ClonePolicyNotImplemented);

class MedicalMetadataParser : public core::IParser
{
    UNIQUE_ID("{9D535CC5-40B9-46B0-8F58-12999E993B32}", "Medical Metadata Parser");
public:
    MedicalMetadataParser();

    virtual ~MedicalMetadataParser();

    virtual void parseFile(const core::Filesystem::Path& path);

    virtual IParser* create();

    virtual void getSupportedExtensions(core::IParser::Extensions &) const;

    const communication::MedicalMetaData::MetaData& getMetadata() const;

    virtual void getObjects(core::Objects& objects);

private:
    core::Filesystem::Path path;
    core::ObjectWrapperPtr object;
    core::shared_ptr<communication::MedicalMetaData::MetaData> metadata;
};

typedef core::shared_ptr<MedicalMetadataParser> MedicalMetadataParserPtr;
typedef core::shared_ptr<const MedicalMetadataParser> MedicalMetadataParserConstPtr;
typedef core::weak_ptr<MedicalMetadataParser> MedicalMetadataParserWeakPtr;

#endif //HEADER_GUARD_COMMUNICATION_METADATAPARSER_H__
