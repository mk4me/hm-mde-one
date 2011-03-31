#ifndef HEADER_GUARD_OSGSCHEMEDRAWER_H__
#define HEADER_GUARD_OSGSCHEMEDRAWER_H__

#include "ISchemeDrawer.h"

//! Klasa bazowa dla schematow osg specyficznych
class OsgSchemeDrawer : public ISchemeDrawer
{
public:
    //! metoda powinna zwracac wezel 
    virtual osg::ref_ptr<osg::Node> getNode() = 0;
    virtual void init(SkeletalVisualizationSchemeConstPtr scheme) { visualizationScheme = scheme; }

protected:
    SkeletalVisualizationSchemeConstPtr getVisualiztionScheme() const { return visualizationScheme; }

private:
    SkeletalVisualizationSchemeConstPtr visualizationScheme;
};

typedef boost::shared_ptr<OsgSchemeDrawer> OsgSchemeDrawerPtr;

#endif //HEADER_GUARD_OSGSCHEMEDRAWER_H__