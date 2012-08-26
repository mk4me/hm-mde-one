#ifndef HEADER_GUARD_SCHEMEDRAWERCONTAINER_H__
#define HEADER_GUARD_SCHEMEDRAWERCONTAINER_H__
#include <osg/Geode>
#include <vector>
#include <boost/range.hpp>
#include <plugins/kinematic/SkeletalVisualizationScheme.h>
#include "OsgSchemeDrawer.h"

//! Klasa realizuje wzorzec 'kompozyt'
//! pozwala przechowywać wiecej drawerów pod sobą
class SchemeDrawerContainer : public OsgSchemeDrawer
{
public:
    //! pozwala przechodzić i modyfikować kolekcję drawerów
    typedef std::vector<OsgSchemeDrawerPtr>::iterator iterator;
    //! pozwala przechodzić kolekcję drawerów
    typedef std::vector<OsgSchemeDrawerPtr>::const_iterator const_iterator;
    //! modyfikowalny zakres drawerów
    typedef boost::iterator_range<iterator> range;
    //! niemodyfikowalny zakres drawerów
    typedef boost::iterator_range<const_iterator> const_range;

public:
    //! Konstruktor
    SchemeDrawerContainer();

public:
    //! return węzeł do którego podpinane są liście
    virtual osg::ref_ptr<osg::Node> getNode();
    //! inicjalizacja drawera
    //! \param scheme schemat wizualizacji z danymi
    virtual void init(VisualizationSchemeConstPtr scheme);
    //! wywoływane przed zakończeniem pracy schematu wizulalizacji
    virtual void deinit();
    //! wywoływane cyklicznie odrysowanie (nie używane)
    virtual void draw();
    //! wywoływane, gdy schemat zmienił swój stan
    virtual void update();

public:
    //! Dodanie kolejnego drawera do kolecji, nie może on już w tej kolekcji figurować
    //! \param drawer dodawany drawer, jeśli już istnieje to wywoła się asercja
    void addDrawer(OsgSchemeDrawerPtr drawer);
    //! Usunięcie drawera z kolekcji, drawer musi w kolekcji istnieć
    //! \param drawer usuwany drawer, jeśli go nie ma to wywołana zostnie asercja
    void removeDrawer(OsgSchemeDrawerPtr drawer);
    //! \return zwraca wszystkie potomne drawery
    range getDrawers();
    //! \return zwraca wszystkie potomne drawery
    const_range getDrawers() const;

private:
    //! kolekcja z drawerami
    std::vector<OsgSchemeDrawerPtr> drawers;
    //! grupa, pod którą podpięte zostaną węzły drawerów
    osg::ref_ptr<osg::Group> nodes;
    //! czy już zainicjalizowano
    bool wasInit;
};

typedef core::shared_ptr<SchemeDrawerContainer> SchemeDrawerContainerPtr;

#endif
