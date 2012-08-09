#ifndef HEADER_GUARD_SCHEMEDRAWERCONTAINER_H__
#define HEADER_GUARD_SCHEMEDRAWERCONTAINER_H__
#include <osg/Geode>
#include <vector>
#include <boost/range.hpp>
#include <plugins/kinematic/SkeletalVisualizationScheme.h>
#include "OsgSchemeDrawer.h"

//! Klasa realizuje wzorzec 'kompozyt'
//! pozwala przechowywac wiecej drawerow pod soba
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
    //! reuturn
    virtual osg::ref_ptr<osg::Node> getNode();
    //! inicjalizacja drawera
    //! \param scheme schemat wizualizacji z danymi
    virtual void init(VisualizationSchemeConstPtr scheme);
    //! wywolywane przed zakonczeniem pracy schematu wizulalizacji
    virtual void deinit();
    //! wywolywane cyklicznie odrysowanie (nie uzywane)
    virtual void draw();
    //! wywolywane, gdy schemat zmienil swoj stan
    virtual void update();

public:
    //! Dodanie kolejnego drawera do kolecji, nie moze on juz w tej kolekcji figurowac
    //! \param drawer dodawany drawer, jesli juz istnieje to wywola sie asercja
    void addDrawer(OsgSchemeDrawerPtr drawer);
    //! Usuniecie drawera z kolekcji, drawer musi w kolekcji istniec
    //! \param drawer usuwany drawer, jesli go nie ma to wywolana zostnie asercja
    void removeDrawer(OsgSchemeDrawerPtr drawer);
    //! \return zwraca wszystkie potomne drawery
    range getDrawers();
    //! \return zwraca wszystkie potomne drawery
    const_range getDrawers() const;

private:
    //! kolekcja z drawerami
    std::vector<OsgSchemeDrawerPtr> drawers;
    //! grupa, pod ktora podpiete zostana wezly drawerow
    osg::ref_ptr<osg::Group> nodes;
    //! czy juz zainicjalizowano
    bool wasInit;
};

typedef core::shared_ptr<SchemeDrawerContainer> SchemeDrawerContainerPtr;

#endif
