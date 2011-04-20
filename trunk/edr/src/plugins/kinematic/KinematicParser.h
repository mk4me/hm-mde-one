#ifndef __HEADER_GUARD_KINEMATIC__KINEMATICPARSER_H__
#define __HEADER_GUARD_KINEMATIC__KINEMATICPARSER_H__

#include <core/IParser.h>
#include <core/IDataManager.h>

class KinematicParser : public core::IParser
{
    UNIQUE_ID("{AE673002-AE76-4E79-983E-ABDB15713E74}", "Kinematic parser");
private:
    core::ObjectWrapperPtr kinematicMarkersSkeleton;
    core::ObjectWrapperPtr schemeMarkersSkeleton;
    core::ObjectWrapperPtr kinematicMarkers;
    core::ObjectWrapperPtr schemeMarkers;

public:
    KinematicParser();
    virtual ~KinematicParser();
// IParser
public:
    virtual void parseFile(core::IDataManager* dataManager, const boost::filesystem::path& path);
    virtual core::IParser* create();
    virtual std::string getSupportedExtensions() const;
    virtual void getObjects(std::vector<core::ObjectWrapperPtr>& objects);
};
#endif  // __HEADER_GUARD_KINEMATIC__KINEMATICPARSER_H__