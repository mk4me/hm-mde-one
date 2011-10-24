#ifndef __HEADER_GUARD_KINEMATIC__KINEMATICPARSER_H__
#define __HEADER_GUARD_KINEMATIC__KINEMATICPARSER_H__

#include <core/IParser.h>
#include <core/IDataManager.h>

class KinematicParser : public core::IParser
{
    UNIQUE_ID("{AE673002-AE76-4E79-983E-ABDB15713E74}", "Kinematic parser");
private:
    //core::ObjectWrapperPtr kinematicMarkersSkeleton;
    //core::ObjectWrapperPtr schemeMarkersSkeleton;
    core::ObjectWrapperPtr skeletonData;
    //core::ObjectWrapperPtr kinematicMarkers;
    //core::ObjectWrapperPtr schemeMarkers;

public:
    KinematicParser();
    virtual ~KinematicParser();
// IParser
public:
    virtual void parseFile(const core::Filesystem::Path& path);
    virtual core::IParser* create();
    virtual void getSupportedExtensions(Extensions & extensions) const;
    virtual void getObjects(core::Objects& objects);
};

class AsfParser : public core::IParser
{
	UNIQUE_ID("{0E3B8309-AA5B-4ECD-B941-8FA64F8C9625}", "Asf parser");
private:
	core::ObjectWrapperPtr skeletalModel;

public:
	AsfParser();
	virtual ~AsfParser();
	// IParser
public:
	virtual void parseFile(const core::Filesystem::Path& path);
	virtual core::IParser* create();
    virtual void getSupportedExtensions(Extensions & extensions) const;
    virtual void getObjects(core::Objects& objects);
};
#endif  // __HEADER_GUARD_KINEMATIC__KINEMATICPARSER_H__