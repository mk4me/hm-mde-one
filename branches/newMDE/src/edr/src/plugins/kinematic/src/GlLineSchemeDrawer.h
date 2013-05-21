#ifndef HEADER_GUARD_GLLINESCHEMEDRAWER_H__
#define HEADER_GUARD_GLLINESCHEMEDRAWER_H__
#include <map>
#include <osg/Geode>
#include <osg/Vec4f>
#include"SkeletalVisualizationScheme.h"
#include "OsgSchemeDrawer.h"

//! Schemat odrysowywujący połączenia pomiędzy punktami, połączenia są w formie cylindrów
class GlLineSchemeDrawer : public OsgSchemeDrawer
{
public:
    //! Konstruktor
    //! \param cylinderComplexity złożonosc pojedynczego cylindra (l. wierzchołków podstawy)
    //! \param cylinderRadius promień pojedynczego cylindra
    GlLineSchemeDrawer(int cylinderComplexity, float cylinderRadius);
    //! Konstruktor
    //! \param cylinderComplexity złożonosc pojedynczego cylindra (l. wierzchołków podstawy)
    //! \param cylinderRadius promień pojedynczego cylindra
    //! \param color domyślny kolor tworzonych cylindrów
    GlLineSchemeDrawer(int cylinderComplexity, float cylinderRadius, const osg::Vec4& color);

public:
    //! Realizacja metody abstrakcyjnej (nieużywana)
    virtual void draw();
    //! Wywoływane, gdy schemat został zmieniony i jego wizualizacja wymaga odswieżenia
    virtual void update();
    //! Inicjalizacja drawera, tworzone są węzły
    //! \param scheme Dostarczany schemat wizualizacji
    virtual void init(VisualizationSchemeConstPtr scheme);
    //! Wywoływane przy zakończeniu działania, sprzątnięcie obiektów ze sceny
    virtual void deinit();
    //! \return zwraca węzeł ze stworzonymi połączeniami
    virtual osg::ref_ptr<osg::Node> getNode() { return node; }

private:
    typedef osg::ref_ptr<osg::Geode> GeodePtr;
    //! wskaźnik do geometrii, która trafia do węzła
    typedef osg::ref_ptr<osg::Geometry> GeometryPtr;
    //! wskaźnik do węzła zawierającego transformację
    typedef osg::ref_ptr<osg::PositionAttitudeTransform> TransformPtr;

private:
    //! Metoda tworzy węzeł z cylindrem
    //! \param complex złożoność cylindra (l. wierzchołków podstawy)
    //! \param height długość / wysokość cylindra
    //! \param radius promień cylindra
    //! \param color kolor cylindra
    //! \return węzeł z nowo utworzonym cylindrem
    GeometryPtr createCustomCylinder( int complex , float height, float radius, osg::Vec4 color) const;
    //! Metoda towrzy cylinder oraz nadaje mu właściwą transformację
    //! \param from punkt, w którym cylinder się zaczyna
    //! \param to punkt, do którego cylinder zmierza
    //! \param color kolor cylindra
    //! \param length długość cylindra
    TransformPtr addTransform(const osg::Vec3& from, const osg::Vec3& to, const osg::Vec4& color, float length);

private:
    //! Kolekcja ze stworzonymi cylindrami
    std::vector<TransformPtr> cones;
    //! węzeł ze stworzonymi cylindrami
    osg::ref_ptr<osg::Group> node;
    //! złożoność pojedynczego cylindrami
    int complex;
    //! promień cylindra
    float radius;
    //! flaga oznaczająca, czy przy wizualizacji ma być użyty kolor użytkownika
    bool useCustomColor;
    //! kolor wybrany przez użytkownika
    osg::Vec4 customColor;
};
typedef utils::shared_ptr<GlLineSchemeDrawer> GlLineSchemeDrawerPtr;
typedef utils::shared_ptr<const GlLineSchemeDrawer> GlLineSchemeDrawerConstPtr;

#endif //HEADER_GUARD_POINTSCHEMEDRAWER_H__
