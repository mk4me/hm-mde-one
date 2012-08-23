#ifndef HEADER_GUARD_GLLINESCHEMEDRAWER_H__
#define HEADER_GUARD_GLLINESCHEMEDRAWER_H__
#include <map>
#include <osg/Geode>
#include <osg/Vec4f>
#include <plugins/kinematic/SkeletalVisualizationScheme.h>
#include "OsgSchemeDrawer.h"

//! Schemat odrysowywuj¹cy po³¹czenia pomiêdzy punktami, po³¹czenia s¹ w formie cylindrów
class GlLineSchemeDrawer : public OsgSchemeDrawer
{
public:
    //! Konstruktor
    //! \param cylinderComplexity z³o¿onosc pojedynczego cylindra (l. wierzcho³ków podstawy)
    //! \param cylinderRadius promieñ pojedynczego cylindra
    GlLineSchemeDrawer(int cylinderComplexity, float cylinderRadius);
    //! Konstruktor
    //! \param cylinderComplexity z³o¿onosc pojedynczego cylindra (l. wierzcho³ków podstawy)
    //! \param cylinderRadius promieñ pojedynczego cylindra
    //! \param color domyœlny kolor tworzonych cylindrów
    GlLineSchemeDrawer(int cylinderComplexity, float cylinderRadius, const osg::Vec4& color);

public:
    //! Realizacja metody abstrakcyjnej (nieu¿ywana)
    virtual void draw();
    //! Wywo³ywane, gdy schemat zosta³ zmieniony i jego wizualizacja wymaga odswie¿enia
    virtual void update();
    //! Inicjalizacja drawera, tworzone s¹ wêz³y
    //! \param scheme Dostarczany schemat wizualizacji
    virtual void init(VisualizationSchemeConstPtr scheme);
    //! Wywo³ywane przy zakoñczeniu dzia³ania, sprz¹tniêcie obiektów ze sceny
    virtual void deinit();
    //! \return zwraca wêze³ ze stworzonymi po³¹czeniami
    virtual osg::ref_ptr<osg::Node> getNode() { return node; }

private:
    typedef osg::ref_ptr<osg::Geode> GeodePtr;
    //! wskaŸnik do geometrii, która trafia do wêz³a
    typedef osg::ref_ptr<osg::Geometry> GeometryPtr;
    //! wskaŸnik do wêz³a zawieraj¹cego transformacjê
    typedef osg::ref_ptr<osg::PositionAttitudeTransform> TransformPtr;

private:
    //! Metoda tworzy wêze³ z cylindrem
    //! \param complex z³o¿onoœæ cylindra (l. wierzcho³ków podstawy)
    //! \param height d³ugoœæ / wysokoœæ cylindra
    //! \param radius promieñ cylindra
    //! \param color kolor cylindra
    //! \return wêze³ z nowo utworzonym cylindrem
    GeometryPtr createCustomCylinder( int complex , float height, float radius, osg::Vec4 color) const;
    //! Metoda towrzy cylinder oraz nadaje mu w³aœciw¹ transformacjê
    //! \param from punkt, w którym cylinder siê zaczyna
    //! \param to punkt, do którego cylinder zmierza
    //! \param color kolor cylindra
    //! \param length d³ugoœæ cylindra
    TransformPtr addTransform(const osg::Vec3& from, const osg::Vec3& to, const osg::Vec4& color, float length);

private:
    //! Kolekcja ze stworzonymi cylindrami
    std::vector<TransformPtr> cones;
    //! wêze³ ze stworzonymi cylindrami
    osg::ref_ptr<osg::Group> node;
    //! z³o¿onoœæ pojedynczego cylindrami
    int complex;
    //! promieñ cylindra
    float radius;
    //! flaga oznaczaj¹ca, czy przy wizualizacji ma byæ u¿yty kolor u¿ytkownika
    bool useCustomColor;
    //! kolor wybrany przez u¿ytkownika
    osg::Vec4 customColor;
};
typedef core::shared_ptr<GlLineSchemeDrawer> GlLineSchemeDrawerPtr;
typedef core::shared_ptr<const GlLineSchemeDrawer> GlLineSchemeDrawerConstPtr;

#endif //HEADER_GUARD_POINTSCHEMEDRAWER_H__
