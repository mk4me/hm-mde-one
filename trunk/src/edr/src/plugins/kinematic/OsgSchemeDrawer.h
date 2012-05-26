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
    //! ustawia widocznosc calego drawera
    //! \param visible czy widoczny
    virtual void setVisible(bool visible) 
    {
        getNode()->setNodeMask(visible ? 0xFFFF : 0);
    }
    //! klasa pochodna powinna wykonac najpierw ta metode a pozniej dokonac reszty inicjalizacji
    //! \param scheme schemat przechowywany na wlsanosc
    virtual void init(VisualizationSchemeConstPtr scheme) 
    { 
        UTILS_ASSERT(initialized == false, "Drawer was previously initialized");
        visualizationScheme = scheme; 
        initialized = true;
    }
    //! \return czy drawer zostal zainicjalizowany
    bool isInitialized() const { return initialized; }

protected:
	typedef osg::ref_ptr<osg::Geode> GeodePtr;
	typedef osg::ref_ptr<osg::Geometry> GeometryPtr;

public:
    //! return schemat wizualizacji, ktory jest odrysowywany
    VisualizationSchemeConstPtr getVisualiztionScheme() const { return visualizationScheme; }

private:
    //! schemat wizualizacji, ktory jest odrysowywany
    VisualizationSchemeConstPtr visualizationScheme;
    //! czy drawer zostal juz zainicjalizowany
    bool initialized;
};

typedef core::shared_ptr<OsgSchemeDrawer> OsgSchemeDrawerPtr;

#endif //HEADER_GUARD_OSGSCHEMEDRAWER_H__