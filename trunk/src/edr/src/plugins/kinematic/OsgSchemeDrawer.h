#ifndef HEADER_GUARD_OSGSCHEMEDRAWER_H__
#define HEADER_GUARD_OSGSCHEMEDRAWER_H__

#include "ISchemeDrawer.h"
//! Klasa bazowa dla schematow osg specyficznych
class OsgSchemeDrawer : public ISchemeDrawer
{
public:
    OsgSchemeDrawer() : initialized(false) {}
public:
    //! metoda powinna zwracac wezel 
    virtual osg::ref_ptr<osg::Node> getNode() = 0;

    virtual void setVisible(bool visible) 
    {
        getNode()->setNodeMask(visible ? 0xFFFF : 0);
    }

    virtual void init(VisualizationSchemeConstPtr scheme) 
    { 
        UTILS_ASSERT(initialized == false, "Drawer was previously initialized");
        visualizationScheme = scheme; 
        initialized = true;
    }

    bool isInitialized() const { return initialized; }

protected:
	typedef osg::ref_ptr<osg::Geode> GeodePtr;
	typedef osg::ref_ptr<osg::Geometry> GeometryPtr;

public:
    VisualizationSchemeConstPtr getVisualiztionScheme() const { return visualizationScheme; }

private:
    VisualizationSchemeConstPtr visualizationScheme;
    bool initialized;
};

typedef core::shared_ptr<OsgSchemeDrawer> OsgSchemeDrawerPtr;

#endif //HEADER_GUARD_OSGSCHEMEDRAWER_H__