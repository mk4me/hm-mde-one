#ifndef HEADER_GUARD_OSGSCHEMEDRAWER_H__
#define HEADER_GUARD_OSGSCHEMEDRAWER_H__

#include "ISchemeDrawer.h"

//! Klasa bazowa dla schemat�w osg specyficznych
class OsgSchemeDrawer : public ISchemeDrawer
{
public:
    //! konstruktor
    OsgSchemeDrawer() : initialized(false) {}

public:
    //! metoda powinna zwraca� w�ze� 
    virtual osg::ref_ptr<osg::Node> getNode() = 0;
    //! ustawia widoczno�� ca�ego drawera
    //! \param visible czy widoczny
    virtual void setVisible(bool visible) 
    {
        getNode()->setNodeMask(visible ? 0xFFFF : 0);
    }
    //! klasa pochodna powinna wykona� najpierw t� metode a p�niej dokona� reszty inicjalizacji
    //! \param scheme schemat przechowywany na w�asno��
    virtual void init(VisualizationSchemeConstPtr scheme) 
    { 
        UTILS_ASSERT(initialized == false, "Drawer was previously initialized");
        visualizationScheme = scheme; 
        initialized = true;
    }
    //! \return czy drawer zosta� zainicjalizowany
    bool isInitialized() const { return initialized; }

protected:
    //! wska�nik do w�z�a przechowywuj�cego geometri�
    typedef osg::ref_ptr<osg::Geode> GeodePtr;
    //! wska�nik do geometrii, kt�ra trafia do w�z�a
    typedef osg::ref_ptr<osg::Geometry> GeometryPtr;

public:
    //! return schemat wizualizacji, kt�ry jest odrysowywany
    VisualizationSchemeConstPtr getVisualiztionScheme() const { return visualizationScheme; }

private:
    //! schemat wizualizacji, kt�ry jest odrysowywany
    VisualizationSchemeConstPtr visualizationScheme;
    //! czy drawer zosta� ju� zainicjalizowany
    bool initialized;
};

typedef core::shared_ptr<OsgSchemeDrawer> OsgSchemeDrawerPtr;

#endif //HEADER_GUARD_OSGSCHEMEDRAWER_H__