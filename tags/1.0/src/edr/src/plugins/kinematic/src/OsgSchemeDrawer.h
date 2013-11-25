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
    //! metoda powinna zwracać węzeł 
    virtual osg::ref_ptr<osg::Node> getNode() = 0;
    //! ustawia widoczność całego drawera
    //! \param visible czy widoczny
    virtual void setVisible(bool visible) 
    {
        getNode()->setNodeMask(visible ? 0xFFFF : 0);
    }
    //! klasa pochodna powinna wykonać najpierw tą metode a później dokonać reszty inicjalizacji
    //! \param scheme schemat przechowywany na własność
    virtual void init(VisualizationSchemeConstPtr scheme) 
    { 
        UTILS_ASSERT(initialized == false, "Drawer was previously initialized");
        visualizationScheme = scheme; 
        initialized = true;
    }
    //! \return czy drawer został zainicjalizowany
    bool isInitialized() const { return initialized; }

protected:
    //! wskaźnik do węzła przechowywującego geometrię
    typedef osg::ref_ptr<osg::Geode> GeodePtr;
    //! wskaźnik do geometrii, która trafia do węzła
    typedef osg::ref_ptr<osg::Geometry> GeometryPtr;

public:
    //! return schemat wizualizacji, który jest odrysowywany
    VisualizationSchemeConstPtr getVisualiztionScheme() const { return visualizationScheme; }

private:
    //! schemat wizualizacji, który jest odrysowywany
    VisualizationSchemeConstPtr visualizationScheme;
    //! czy drawer został już zainicjalizowany
    bool initialized;
};

typedef utils::shared_ptr<OsgSchemeDrawer> OsgSchemeDrawerPtr;

#endif //HEADER_GUARD_OSGSCHEMEDRAWER_H__
