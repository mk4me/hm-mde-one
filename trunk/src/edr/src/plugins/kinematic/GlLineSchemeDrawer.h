#ifndef HEADER_GUARD_GLLINESCHEMEDRAWER_H__
#define HEADER_GUARD_GLLINESCHEMEDRAWER_H__
#include <map>
#include <osg/Geode>
#include <osg/Vec4f>
#include <plugins/kinematic/skeletalVisualizationScheme.h>	
#include "OsgSchemeDrawer.h"

//! Schemat odrysowywujacy polaczenia pomiedzy punktami, polaczenia sa w formie cylindrow
class GlLineSchemeDrawer : public OsgSchemeDrawer
{
public:
    //! Konstruktor
    //! \param cylinderComplexity zlozonosc pojedynczego cylindra (l. wierzcholkow podstawy)
    //! \param cylinderRadius promien pojedynczego cylindra
    GlLineSchemeDrawer(int cylinderComplexity, float cylinderRadius);
    //! Konstruktor
    //! \param cylinderComplexity zlozonosc pojedynczego cylindra (l. wierzcholkow podstawy)
    //! \param cylinderRadius promien pojedynczego cylindra
    //! \param color domyslny kolor tworzonych cylindrow
    GlLineSchemeDrawer(int cylinderComplexity, float cylinderRadius, const osg::Vec4& color);

public:
    //! Realizacja metody abstrakcyjnej (nieuzywana)
    virtual void draw();
    //! Wywolywane, gdy schemat zostal zmieniony i jego wizualizacja wymaga odswiezenia
    virtual void update();
    //! Inicjalizacja drawera, tworzone sa wezly
    //! \param scheme Dostarczany schemat wizualizacji
    virtual void init(VisualizationSchemeConstPtr scheme);
    //! Wywolywane przy zakonczeniu dzialania, sprzatniecie obiektow ze sceny
    virtual void deinit();
    //! \return zwraca noda ze stworzonymi polaczeniami
    virtual osg::ref_ptr<osg::Node> getNode() { return node; }

private:
    typedef osg::ref_ptr<osg::Geode> GeodePtr;
    typedef osg::ref_ptr<osg::Geometry> GeometryPtr;
    typedef osg::ref_ptr<osg::PositionAttitudeTransform> TransformPtr;

private:
    //! Metoda tworzy wezel z cylindrem
    //! \param complex zlozonosc cylindra (l. wierzcholkow podstawy)
    //! \param height dlugosc / wysokosc cylindra
    //! \param radius promien cylindra
    //! \param color kolor cylindra
    //! \return wezel z nowo utworzonym cylindrem
    GeometryPtr createCustomCylinder( int complex , float height, float radius, osg::Vec4 color) const;
    //! Metoda towrzy cylinder oraz nadaje mu wlasciwa transformacje
    //! \param from punkt, w ktorym cylinder sie zaczyna
    //! \param to punkt, do ktorego cylinder zmierza
    //! \param color kolor cylindra
    //! \param length dlugosc cylindra
    TransformPtr addTransform(const osg::Vec3& from, const osg::Vec3& to, const osg::Vec4& color, float length);

private:
    //! Kolekcja ze stworzonymi cylindrami
    std::vector<TransformPtr> cones;
    //! Wezel ze stworzonymi cylindrami
    osg::ref_ptr<osg::Group> node;
    //! zlozonej pojedynczego cylindrami 
    int complex;
    //! promien cylindra
    float radius;
    //! flaga oznaczajaca, czy przy wizualizacji ma byc uzyty kolor uzytkownika
    bool useCustomColor;
    //! kolor wybrany przez uzytkownika
    osg::Vec4 customColor;
};
typedef core::shared_ptr<GlLineSchemeDrawer> GlLineSchemeDrawerPtr;
typedef core::shared_ptr<const GlLineSchemeDrawer> GlLineSchemeDrawerConstPtr;

#endif //HEADER_GUARD_POINTSCHEMEDRAWER_H__