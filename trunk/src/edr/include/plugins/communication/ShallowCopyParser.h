#ifndef HEADER_GUARD_COMMUNICATION_SHALLOWCOPYPARSER_H__
#define HEADER_GUARD_COMMUNICATION_SHALLOWCOPYPARSER_H__

#include <core/IParser.h>
#include <plugins/communication/Entity.h>
#include <core/IDataManager.h>

CORE_DEFINE_WRAPPER(communication::MotionShallowCopy::ShallowCopy, utils::PtrPolicyBoost, utils::ClonePolicyNotImplemented);

typedef core::shared_ptr<communication::MotionShallowCopy::ShallowCopy> MotionShallowCopyPtr;
typedef core::shared_ptr<const communication::MotionShallowCopy::ShallowCopy> MotionShallowCopyConstPtr;

class MotionShallowCopyParser : public core::IParser
{
    UNIQUE_ID("{C11AD1FE-B44F-4F6B-80EE-B51DCE1758E8}", "Motion Shallow Copy Parser");
public:
    MotionShallowCopyParser();

    virtual ~MotionShallowCopyParser();
    
    virtual void parseFile(const core::Filesystem::Path& path);

    virtual IParser* create();

    virtual void getSupportedExtensions(core::IParser::Extensions & extensions) const;

    const MotionShallowCopyPtr& getShallowCopy();
    const MotionShallowCopyConstPtr& getShallowCopy() const;

    virtual void getObjects(core::Objects& objects);

private:
    core::Filesystem::Path path;
    core::ObjectWrapperPtr object;
    MotionShallowCopyPtr shallowCopy;
    MotionShallowCopyConstPtr constShallowCopy;
};

typedef core::shared_ptr<MotionShallowCopyParser> MotionShallowCopyParserPtr;
typedef core::shared_ptr<const MotionShallowCopyParser> MotionShallowCopyParserConstPtr;
typedef core::weak_ptr<MotionShallowCopyParser> MotionShallowCopyParserWeakPtr;


CORE_DEFINE_WRAPPER(communication::MedicalShallowCopy::ShallowCopy, utils::PtrPolicyBoost, utils::ClonePolicyNotImplemented);

typedef core::shared_ptr<communication::MedicalShallowCopy::ShallowCopy> MedicalShallowCopyPtr;
typedef core::shared_ptr<const communication::MedicalShallowCopy::ShallowCopy> MedicalShallowCopyConstPtr;

class MedicalShallowCopyParser : public core::IParser
{
    UNIQUE_ID("{C11AD1FE-B44F-4F6B-8EEE-B51DCE1758E8}", "Medical Shallow Copy Parser");
public:
    MedicalShallowCopyParser();

    virtual ~MedicalShallowCopyParser();

    virtual void parseFile(const core::Filesystem::Path& path);

    virtual IParser* create();

    virtual void getSupportedExtensions(core::IParser::Extensions & extensions) const;

    const MedicalShallowCopyPtr& getShallowCopy();
    const MedicalShallowCopyConstPtr& getShallowCopy() const;

    virtual void getObjects(core::Objects& objects);

private:
    core::Filesystem::Path path;
    core::ObjectWrapperPtr object;
    MedicalShallowCopyPtr shallowCopy;
    MedicalShallowCopyConstPtr constShallowCopy;
};

typedef core::shared_ptr<MedicalShallowCopyParser> MedicalShallowCopyParserPtr;
typedef core::shared_ptr<const MedicalShallowCopyParser> MedicalShallowCopyParserConstPtr;
typedef core::weak_ptr<MedicalShallowCopyParser> MedicalShallowCopyParserWeakPtr;


#endif //HEADER_GUARD_COMMUNICATION_SHALLOWCOPYPARSER_H__
