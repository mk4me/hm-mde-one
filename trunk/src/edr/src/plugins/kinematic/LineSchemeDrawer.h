#ifndef HEADER_GUARD_LINESCHEMEDRAWER_H__
#define HEADER_GUARD_LINESCHEMEDRAWER_H__

#include <core/SmartPtr.h>
#include <osg/PositionAttitudeTransform>
#include "OsgSchemeDrawer.h"

//! Schemat odrysowywuj�cy linie szkieletu
class LineSchemeDrawer : public OsgSchemeDrawer
{
public:
    //! Konstruktor
    LineSchemeDrawer() {}

public:
    //! Realizacja metody abstrakcyjnej (nieu�ywana)
    virtual void draw();
    //! Wywo�ywane, gdy schemat zosta� zmieniony i jego wizualizacja wymaga odswie�enia
    virtual void update();
    //! Wywo�ywane przy zako�czeniu dzia�ania, sprz�tni�cie obiekt�w ze sceny
    virtual void deinit();
    //! Inicjalizacja drawera, tworzone s� w�z�y
    //! \param scheme Dostarczany schemat wizualizacji
    virtual void init(VisualizationSchemeConstPtr scheme);
    //! \return zwraca w�ze� ze stworzonymi po��czeniami
    virtual osg::ref_ptr<osg::Node> getNode();

private:
    //! wska�nik do w�z�a przechowywuj�cego geometri�
    typedef osg::ref_ptr<osg::Geode> GeodePtr;
    //! wska�nik do w�z�a zawieraj�cego transformacj�
    typedef osg::ref_ptr<osg::PositionAttitudeTransform> TransformPtr;
    //! po��czenie pomi�dzy stawami lub markerami
    typedef VisualizationScheme::Connection Connection;
    //! aktualny stan markera (kolor, pozycja, ...)
    typedef VisualizationScheme::State JointState;

private:
    //! Metoda tworzy lini� pomi�dzy punktami
    //! \param from pocz�tek linii  
    //! \param to koniec linii
    //! \param color kolor linii
    void addLine(const osg::Vec3& from, const osg::Vec3& to, const osg::Vec4& color);

private:
    //! tutaj trafiaj� tworzone linie
    std::vector<GeodePtr> lines;
    //! bufory z wierzcho�kami dla linii
    std::vector< osg::ref_ptr<osg::Vec3Array> > buffers;
    //! w�ze� ze stworzonymi liniami
    osg::ref_ptr<osg::Group> node;
};

#endif
