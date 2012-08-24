#ifndef HEADER_GUARD_SCHEMEDRAWERCONTAINER_H__
#define HEADER_GUARD_SCHEMEDRAWERCONTAINER_H__
#include <osg/Geode>
#include <vector>
#include <boost/range.hpp>
#include <plugins/kinematic/SkeletalVisualizationScheme.h>
#include "OsgSchemeDrawer.h"

//! Klasa realizuje wzorzec 'kompozyt'
//! pozwala przechowywa� wiecej drawer�w pod sob�
class SchemeDrawerContainer : public OsgSchemeDrawer
{
public:
    //! pozwala przechodzi� i modyfikowa� kolekcj� drawer�w
    typedef std::vector<OsgSchemeDrawerPtr>::iterator iterator;
    //! pozwala przechodzi� kolekcj� drawer�w
    typedef std::vector<OsgSchemeDrawerPtr>::const_iterator const_iterator;
    //! modyfikowalny zakres drawer�w
    typedef boost::iterator_range<iterator> range;
    //! niemodyfikowalny zakres drawer�w
    typedef boost::iterator_range<const_iterator> const_range;

public:
    //! Konstruktor
    SchemeDrawerContainer();

public:
    //! return w�ze� do kt�rego podpinane s� li�cie
    virtual osg::ref_ptr<osg::Node> getNode();
    //! inicjalizacja drawera
    //! \param scheme schemat wizualizacji z danymi
    virtual void init(VisualizationSchemeConstPtr scheme);
    //! wywo�ywane przed zako�czeniem pracy schematu wizulalizacji
    virtual void deinit();
    //! wywo�ywane cyklicznie odrysowanie (nie u�ywane)
    virtual void draw();
    //! wywo�ywane, gdy schemat zmieni� sw�j stan
    virtual void update();

public:
    //! Dodanie kolejnego drawera do kolecji, nie mo�e on ju� w tej kolekcji figurowa�
    //! \param drawer dodawany drawer, je�li ju� istnieje to wywo�a si� asercja
    void addDrawer(OsgSchemeDrawerPtr drawer);
    //! Usuni�cie drawera z kolekcji, drawer musi w kolekcji istnie�
    //! \param drawer usuwany drawer, je�li go nie ma to wywo�ana zostnie asercja
    void removeDrawer(OsgSchemeDrawerPtr drawer);
    //! \return zwraca wszystkie potomne drawery
    range getDrawers();
    //! \return zwraca wszystkie potomne drawery
    const_range getDrawers() const;

private:
    //! kolekcja z drawerami
    std::vector<OsgSchemeDrawerPtr> drawers;
    //! grupa, pod kt�r� podpi�te zostan� w�z�y drawer�w
    osg::ref_ptr<osg::Group> nodes;
    //! czy ju� zainicjalizowano
    bool wasInit;
};

typedef core::shared_ptr<SchemeDrawerContainer> SchemeDrawerContainerPtr;

#endif
