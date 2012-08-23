#ifndef HEADER_GUARD_OSGSCHEMEDRAWER_H__
#define HEADER_GUARD_OSGSCHEMEDRAWER_H__

#include "ISchemeDrawer.h"

//! Klasa bazowa dla schematów osg specyficznych
class OsgSchemeDrawer : public ISchemeDrawer
{
public:
    //! konstruktor
    OsgSchemeDrawer() : initialized(false) {}

public:
    //! metoda powinna zwracaæ wêze³ 
    virtual osg::ref_ptr<osg::Node> getNode() = 0;
    //! ustawia widocznoœæ ca³ego drawera
    //! \param visible czy widoczny
    virtual void setVisible(bool visible) 
    {
        getNode()->setNodeMask(visible ? 0xFFFF : 0);
    }
    //! klasa pochodna powinna wykonaæ najpierw t¹ metode a póŸniej dokonaæ reszty inicjalizacji
    //! \param scheme schemat przechowywany na w³asnoœæ
    virtual void init(VisualizationSchemeConstPtr scheme) 
    { 
        UTILS_ASSERT(initialized == false, "Drawer was previously initialized");
        visualizationScheme = scheme; 
        initialized = true;
    }
    //! \return czy drawer zosta³ zainicjalizowany
    bool isInitialized() const { return initialized; }

protected:
    //! wskaŸnik do wêz³a przechowywuj¹cego geometriê
    typedef osg::ref_ptr<osg::Geode> GeodePtr;
    //! wskaŸnik do geometrii, która trafia do wêz³a
    typedef osg::ref_ptr<osg::Geometry> GeometryPtr;

public:
    //! return schemat wizualizacji, który jest odrysowywany
    VisualizationSchemeConstPtr getVisualiztionScheme() const { return visualizationScheme; }

private:
    //! schemat wizualizacji, który jest odrysowywany
    VisualizationSchemeConstPtr visualizationScheme;
    //! czy drawer zosta³ ju¿ zainicjalizowany
    bool initialized;
};

typedef core::shared_ptr<OsgSchemeDrawer> OsgSchemeDrawerPtr;

#endif //HEADER_GUARD_OSGSCHEMEDRAWER_H__