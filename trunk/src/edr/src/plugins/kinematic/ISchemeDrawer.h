#ifndef HEADER_GUARD_ISCHEMEDRAWER_H__
#define HEADER_GUARD_ISCHEMEDRAWER_H__

#include <boost/utility.hpp>
#include <plugins/kinematic/VisualizationScheme.h>

//! Interfejs powinien byc zaimplementowany przez warstwe wizualizacji danych 3D.
//! Stworzona klasa powinna czerpac dane ze schematu wizualizacji/
class ISchemeDrawer : boost::noncopyable
{
public:
    //! wywolywane cyklicznie odrysowanie (nie jest konieczne, jesli uzywamy osg)
    virtual void draw() = 0;
    //! wywolywane, gdy schemat zmienil swoj stan
    virtual void update() = 0;
    //! wywolywane przed zakonczeniem pracy schematu wizulalizacji
    virtual void deinit() = 0;
    //! inicjalizacja drawera
    //! \param scheme schemat wizualizacji z danymi
    virtual void init(VisualizationSchemeConstPtr scheme) = 0;
};
#endif //HEADER_GUARD_ISCHEMEDRAWER_H__
