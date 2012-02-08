#ifndef HEADER_GUARD_SCHEMEDRAWERCONTAINER_H__
#define HEADER_GUARD_SCHEMEDRAWERCONTAINER_H__
#include <osg/Geode>
#include <vector>
#include <boost/range.hpp>
#include <plugins/kinematic/skeletalVisualizationScheme.h>
#include "OsgSchemeDrawer.h"

class SchemeDrawerContainer : public OsgSchemeDrawer
{
public:
    typedef std::vector<OsgSchemeDrawerPtr>::iterator iterator;
    typedef std::vector<OsgSchemeDrawerPtr>::const_iterator const_iterator;
    typedef boost::iterator_range<iterator> range;
    typedef boost::iterator_range<const_iterator> const_range;

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
    range getDrawers();
    const_range getDrawers() const;

private:
    std::vector<OsgSchemeDrawerPtr> drawers;
    osg::ref_ptr<osg::Group> nodes;
    bool wasInit;
};

typedef core::shared_ptr<SchemeDrawerContainer> SchemeDrawerContainerPtr;

#endif