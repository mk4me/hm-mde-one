#ifndef HEADER_GUARD_LINESCHEMEDRAWER_H__
#define HEADER_GUARD_LINESCHEMEDRAWER_H__

#include <core/SmartPtr.h>
#include <osg/PositionAttitudeTransform>
#include "OsgSchemeDrawer.h"

//! Schemat odrysowywujący linie szkieletu
class LineSchemeDrawer : public OsgSchemeDrawer
{
public:
    //! Konstruktor
    LineSchemeDrawer() {}

public:
    //! Realizacja metody abstrakcyjnej (nieużywana)
    virtual void draw();
    //! Wywoływane, gdy schemat został zmieniony i jego wizualizacja wymaga odswieżenia
    virtual void update();
    //! Wywoływane przy zakończeniu działania, sprzątnięcie obiektów ze sceny
    virtual void deinit();
    //! Inicjalizacja drawera, tworzone są węzły
    //! \param scheme Dostarczany schemat wizualizacji
    virtual void init(VisualizationSchemeConstPtr scheme);
    //! \return zwraca węzeł ze stworzonymi połączeniami
    virtual osg::ref_ptr<osg::Node> getNode();

private:
    //! wskaźnik do węzła przechowywującego geometrię
    typedef osg::ref_ptr<osg::Geode> GeodePtr;
    //! wskaźnik do węzła zawierającego transformację
    typedef osg::ref_ptr<osg::PositionAttitudeTransform> TransformPtr;
    //! połączenie pomiędzy stawami lub markerami
    typedef VisualizationScheme::Connection Connection;
    //! aktualny stan markera (kolor, pozycja, ...)
    typedef VisualizationScheme::State JointState;

private:
    //! Metoda tworzy linię pomiędzy punktami
    //! \param from początek linii  
    //! \param to koniec linii
    //! \param color kolor linii
    void addLine(const osg::Vec3& from, const osg::Vec3& to, const osg::Vec4& color);

private:
    //! tutaj trafiają tworzone linie
    std::vector<GeodePtr> lines;
    //! bufory z wierzchołkami dla linii
    std::vector< osg::ref_ptr<osg::Vec3Array> > buffers;
    //! węzeł ze stworzonymi liniami
    osg::ref_ptr<osg::Group> node;
};

#endif
