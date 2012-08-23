#ifndef HEADER_GUARD_ISCHEMEDRAWER_H__
#define HEADER_GUARD_ISCHEMEDRAWER_H__

#include <boost/utility.hpp>
#include <plugins/kinematic/VisualizationScheme.h>

//! Interfejs powinien byæ zaimplementowany przez warstwe wizualizacji danych 3D.
//! Stworzona klasa powinna czerpaæ dane ze schematu wizualizacji
class ISchemeDrawer : boost::noncopyable
{
public:
    //! wywo³ywane cyklicznie odrysowanie (nie jest konieczne, jeœli u¿ywamy osg)
    virtual void draw() = 0;
    //! wywo³ywane, gdy schemat zmieni³ swój stan
    virtual void update() = 0;
    //! wywo³ywane przed zakoñczeniem pracy schematu wizulalizacji
    virtual void deinit() = 0;
    //! inicjalizacja drawera
    //! \param scheme schemat wizualizacji z danymi
    virtual void init(VisualizationSchemeConstPtr scheme) = 0;
};
#endif //HEADER_GUARD_ISCHEMEDRAWER_H__
