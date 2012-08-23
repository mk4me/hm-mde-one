#ifndef HEADER_GUARD_GLLINESCHEMEDRAWER_H__
#define HEADER_GUARD_GLLINESCHEMEDRAWER_H__
#include <map>
#include <osg/Geode>
#include <osg/Vec4f>
#include <plugins/kinematic/SkeletalVisualizationScheme.h>
#include "OsgSchemeDrawer.h"

//! Schemat odrysowywuj�cy po��czenia pomi�dzy punktami, po��czenia s� w formie cylindr�w
class GlLineSchemeDrawer : public OsgSchemeDrawer
{
public:
    //! Konstruktor
    //! \param cylinderComplexity z�o�onosc pojedynczego cylindra (l. wierzcho�k�w podstawy)
    //! \param cylinderRadius promie� pojedynczego cylindra
    GlLineSchemeDrawer(int cylinderComplexity, float cylinderRadius);
    //! Konstruktor
    //! \param cylinderComplexity z�o�onosc pojedynczego cylindra (l. wierzcho�k�w podstawy)
    //! \param cylinderRadius promie� pojedynczego cylindra
    //! \param color domy�lny kolor tworzonych cylindr�w
    GlLineSchemeDrawer(int cylinderComplexity, float cylinderRadius, const osg::Vec4& color);

public:
    //! Realizacja metody abstrakcyjnej (nieu�ywana)
    virtual void draw();
    //! Wywo�ywane, gdy schemat zosta� zmieniony i jego wizualizacja wymaga odswie�enia
    virtual void update();
    //! Inicjalizacja drawera, tworzone s� w�z�y
    //! \param scheme Dostarczany schemat wizualizacji
    virtual void init(VisualizationSchemeConstPtr scheme);
    //! Wywo�ywane przy zako�czeniu dzia�ania, sprz�tni�cie obiekt�w ze sceny
    virtual void deinit();
    //! \return zwraca w�ze� ze stworzonymi po��czeniami
    virtual osg::ref_ptr<osg::Node> getNode() { return node; }

private:
    typedef osg::ref_ptr<osg::Geode> GeodePtr;
    //! wska�nik do geometrii, kt�ra trafia do w�z�a
    typedef osg::ref_ptr<osg::Geometry> GeometryPtr;
    //! wska�nik do w�z�a zawieraj�cego transformacj�
    typedef osg::ref_ptr<osg::PositionAttitudeTransform> TransformPtr;

private:
    //! Metoda tworzy w�ze� z cylindrem
    //! \param complex z�o�ono�� cylindra (l. wierzcho�k�w podstawy)
    //! \param height d�ugo�� / wysoko�� cylindra
    //! \param radius promie� cylindra
    //! \param color kolor cylindra
    //! \return w�ze� z nowo utworzonym cylindrem
    GeometryPtr createCustomCylinder( int complex , float height, float radius, osg::Vec4 color) const;
    //! Metoda towrzy cylinder oraz nadaje mu w�a�ciw� transformacj�
    //! \param from punkt, w kt�rym cylinder si� zaczyna
    //! \param to punkt, do kt�rego cylinder zmierza
    //! \param color kolor cylindra
    //! \param length d�ugo�� cylindra
    TransformPtr addTransform(const osg::Vec3& from, const osg::Vec3& to, const osg::Vec4& color, float length);

private:
    //! Kolekcja ze stworzonymi cylindrami
    std::vector<TransformPtr> cones;
    //! w�ze� ze stworzonymi cylindrami
    osg::ref_ptr<osg::Group> node;
    //! z�o�ono�� pojedynczego cylindrami
    int complex;
    //! promie� cylindra
    float radius;
    //! flaga oznaczaj�ca, czy przy wizualizacji ma by� u�yty kolor u�ytkownika
    bool useCustomColor;
    //! kolor wybrany przez u�ytkownika
    osg::Vec4 customColor;
};
typedef core::shared_ptr<GlLineSchemeDrawer> GlLineSchemeDrawerPtr;
typedef core::shared_ptr<const GlLineSchemeDrawer> GlLineSchemeDrawerConstPtr;

#endif //HEADER_GUARD_POINTSCHEMEDRAWER_H__
