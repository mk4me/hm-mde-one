#ifndef HEADER_GUARD_SCHEMEDRAWERCONTAINER_H__
#define HEADER_GUARD_SCHEMEDRAWERCONTAINER_H__
#include <osg/Geode>
#include <plugins/kinematic/skeletalVisualizationScheme.h>
#include "OsgSchemeDrawer.h"

class SchemeDrawerContainer : public OsgSchemeDrawer
{
public:
    SchemeDrawerContainer();

public:
    virtual osg::ref_ptr<osg::Node> getNode();
    virtual void init(SkeletalVisualizationSchemeConstPtr scheme);
    virtual void deinit();
    virtual void draw();
    virtual void update();

public:
    void addDrawer(OsgSchemeDrawerPtr drawer);
    void removeDrawer(OsgSchemeDrawerPtr drawer);

private:
    std::vector<OsgSchemeDrawerPtr> drawers;
    osg::ref_ptr<osg::Group> nodes;
    bool wasInit;
};

typedef boost::shared_ptr<SchemeDrawerContainer> SchemeDrawerContainerPtr;

#endif