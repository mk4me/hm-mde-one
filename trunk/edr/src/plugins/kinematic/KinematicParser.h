#ifndef __HEADER_GUARD_KINEMATIC__KINEMATICPARSER_H__
#define __HEADER_GUARD_KINEMATIC__KINEMATICPARSER_H__

#include <core/IParser.h>
#include <core/IDataManager.h>

class KinematicParser : public core::IParser
{
    UNIQUE_ID('KINE','PARS');
private:
    core::ObjectWrapperPtr kinematicMarkers;
    core::ObjectWrapperPtr kinematicSkeleton;
    core::ObjectWrapperPtr kinematicMarkersSkeleton;
    core::ObjectWrapperPtr schemeMarkers;
    core::ObjectWrapperPtr schemeSkeleton;
    core::ObjectWrapperPtr schemeMarkersSkeleton;

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