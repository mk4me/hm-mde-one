#ifndef HEADER_GUARD_ISCHEMEDRAWER_H__
#define HEADER_GUARD_ISCHEMEDRAWER_H__

#include <boost/utility.hpp>
#include <plugins/kinematic/VisualizationScheme.h>

//! Interfejs powinien być zaimplementowany przez warstwe wizualizacji danych 3D.
//! Stworzona klasa powinna czerpać dane ze schematu wizualizacji
class ISchemeDrawer : boost::noncopyable
{
public:
    //! wywoływane cyklicznie odrysowanie (nie jest konieczne, jeśli używamy osg)
    virtual void draw() = 0;
    //! wywoływane, gdy schemat zmienił swój stan
    virtual void update() = 0;
    //! wywoływane przed zakończeniem pracy schematu wizulalizacji
    virtual void deinit() = 0;
    //! inicjalizacja drawera
    //! \param scheme schemat wizualizacji z danymi
    virtual void init(VisualizationSchemeConstPtr scheme) = 0;
};
#endif //HEADER_GUARD_ISCHEMEDRAWER_H__
