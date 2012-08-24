#ifndef HEADER_GUARD_SCHEMEDRAWERCONTAINER_H__
#define HEADER_GUARD_SCHEMEDRAWERCONTAINER_H__
#include <osg/Geode>
#include <vector>
#include <boost/range.hpp>
#include <plugins/kinematic/SkeletalVisualizationScheme.h>
#include "OsgSchemeDrawer.h"

//! Klasa realizuje wzorzec 'kompozyt'
//! pozwala przechowywaæ wiecej drawerów pod sob¹
class SchemeDrawerContainer : public OsgSchemeDrawer
{
public:
    //! pozwala przechodziæ i modyfikowaæ kolekcjê drawerów
    typedef std::vector<OsgSchemeDrawerPtr>::iterator iterator;
    //! pozwala przechodziæ kolekcjê drawerów
    typedef std::vector<OsgSchemeDrawerPtr>::const_iterator const_iterator;
    //! modyfikowalny zakres drawerów
    typedef boost::iterator_range<iterator> range;
    //! niemodyfikowalny zakres drawerów
    typedef boost::iterator_range<const_iterator> const_range;

public:
    //! Konstruktor
    SchemeDrawerContainer();

public:
    //! return wêze³ do którego podpinane s¹ liœcie
    virtual osg::ref_ptr<osg::Node> getNode();
    //! inicjalizacja drawera
    //! \param scheme schemat wizualizacji z danymi
    virtual void init(VisualizationSchemeConstPtr scheme);
    //! wywo³ywane przed zakoñczeniem pracy schematu wizulalizacji
    virtual void deinit();
    //! wywo³ywane cyklicznie odrysowanie (nie u¿ywane)
    virtual void draw();
    //! wywo³ywane, gdy schemat zmieni³ swój stan
    virtual void update();

public:
    //! Dodanie kolejnego drawera do kolecji, nie mo¿e on ju¿ w tej kolekcji figurowaæ
    //! \param drawer dodawany drawer, jeœli ju¿ istnieje to wywo³a siê asercja
    void addDrawer(OsgSchemeDrawerPtr drawer);
    //! Usuniêcie drawera z kolekcji, drawer musi w kolekcji istnieæ
    //! \param drawer usuwany drawer, jeœli go nie ma to wywo³ana zostnie asercja
    void removeDrawer(OsgSchemeDrawerPtr drawer);
    //! \return zwraca wszystkie potomne drawery
    range getDrawers();
    //! \return zwraca wszystkie potomne drawery
    const_range getDrawers() const;

private:
    //! kolekcja z drawerami
    std::vector<OsgSchemeDrawerPtr> drawers;
    //! grupa, pod któr¹ podpiête zostan¹ wêz³y drawerów
    osg::ref_ptr<osg::Group> nodes;
    //! czy ju¿ zainicjalizowano
    bool wasInit;
};

typedef core::shared_ptr<SchemeDrawerContainer> SchemeDrawerContainerPtr;

#endif
