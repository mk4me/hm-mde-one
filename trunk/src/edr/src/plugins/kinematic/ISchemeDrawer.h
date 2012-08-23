#ifndef HEADER_GUARD_ISCHEMEDRAWER_H__
#define HEADER_GUARD_ISCHEMEDRAWER_H__

#include <boost/utility.hpp>
#include <plugins/kinematic/VisualizationScheme.h>

//! Interfejs powinien by� zaimplementowany przez warstwe wizualizacji danych 3D.
//! Stworzona klasa powinna czerpa� dane ze schematu wizualizacji
class ISchemeDrawer : boost::noncopyable
{
public:
    //! wywo�ywane cyklicznie odrysowanie (nie jest konieczne, je�li u�ywamy osg)
    virtual void draw() = 0;
    //! wywo�ywane, gdy schemat zmieni� sw�j stan
    virtual void update() = 0;
    //! wywo�ywane przed zako�czeniem pracy schematu wizulalizacji
    virtual void deinit() = 0;
    //! inicjalizacja drawera
    //! \param scheme schemat wizualizacji z danymi
    virtual void init(VisualizationSchemeConstPtr scheme) = 0;
};
#endif //HEADER_GUARD_ISCHEMEDRAWER_H__
