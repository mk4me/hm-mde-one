#ifndef HEADER_GUARD_ISCHEMEDRAWER_H__
#define HEADER_GUARD_ISCHEMEDRAWER_H__

#include <boost/utility.hpp>
#include <plugins/kinematic/skeletalVisualizationScheme.h>

enum DataToDraw {
    DrawMarkers,
    DrawSkeleton
};

class ISchemeDrawer : boost::noncopyable
{
public:
    virtual void draw() = 0;
    virtual void update() = 0;
    virtual void deinit() = 0;
    virtual void init(SkeletalVisualizationSchemeConstPtr scheme) = 0;
};
#endif //HEADER_GUARD_ISCHEMEDRAWER_H__