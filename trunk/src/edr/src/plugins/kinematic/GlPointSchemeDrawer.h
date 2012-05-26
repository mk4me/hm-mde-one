#ifndef HEADER_GUARD_GLPOINTSCHEMEDRAWER_H__
#define HEADER_GUARD_GLPOINTSCHEMEDRAWER_H__
#include <map>
#include <osg/Geode>
#include <osg/Vec4f>
#include <plugins/kinematic/skeletalVisualizationScheme.h>	
#include "OsgSchemeDrawer.h"

//! Schemat odrysowywujacy punkty
class GlPointSchemeDrawer : public OsgSchemeDrawer
{
public:
    //! Konstruktor
    //! \param sphereComplex zlozonosc pojedynczej sfery
    //! \param sphereRadius promien sfery
    GlPointSchemeDrawer(int sphereComplex, float sphereRadius);
    //! Konstruktor
    //! \param sphereComplex zlozonosc sfery
    //! \param sphereRadius promien sfery
    //! \param color kolor zdefiniowany przez uzytkownika
    GlPointSchemeDrawer(int sphereComplex, float sphereRadius, const osg::Vec4& color);

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
    typedef VisualizationScheme::State MarkerState;

private:
    //! Tworzony wszystkie punkty ze schematu wizualizcji
    //! \param markers kolekcja z markerami
    void createMarkersCrowd(const std::vector<MarkerState>& markers);
    //! Tworzy geode markera (jesli istnieje sfera o danym kolorze to nie tworzy nowej geometrii)
    //! \param color zadany kolor
    //! \param scale skala sfery
    //! \return wskaznik do stworzonej geody
    GeodePtr createMarker(osg::Vec4 color, float scale = 1.0f);
    //! Tworzy wezel markera i ustawia mu odpowiednia transformacje
    //! \param point punkt w przestrzeni, gdzie ma byc umieszczony wezel
    //! \param color zadany kolor
    //! \param name nazwa wezla
    //! \return wezel ze stowrzonym markerem
    TransformPtr addPoint(const osg::Vec3& point, const osg::Vec4& color, const std::string& name = std::string());
    //! Tworzy geometrie ze sfera
    //! \param complex zlozonosc sfery
    //! \param color kolor sfery
    //! \return wskaznik do stworzonej geometrii
    GeometryPtr createCustomSphere(int complex, osg::Vec4 color) const;
    //! Zmienia kolor, jesli kolor geometrii jest inny niz kolor markera. Jesli jest taka potrzeba tworzona jest nowa geometria
    //! \param point wezel z markerem
    //! \param state marker pochadzacy ze schematu wizualizacji
    void changeColorIfNecessary(TransformPtr point, const VisualizationScheme::State& state);

private:
    //! wezel ze stworzonymi markerami
    osg::ref_ptr<osg::Group> node;
    //! struktura ze stworzonymi wezlami markerow
    std::vector<TransformPtr> points;
    //! mapa kolor -> geometria, stworzona w celu optymalizacji
    std::map<osg::Vec4f, GeometryPtr> spheresByColor;
    //! domysla zlozonosc sfery
    int complex;
    //! domyslny promien sfery
    float radius;
    //! domyslny kolor sfery
    osg::Vec4 customColor;
    //! flaga okreslajaca, czy uzywac koloru zdefiniowanego przez uzytkownika
    bool useCustomColor;
};
typedef core::shared_ptr<GlPointSchemeDrawer> GlPointSchemeDrawerPtr;
typedef core::shared_ptr<const GlPointSchemeDrawer> GlPointSchemeDrawerConstPtr;

#endif //HEADER_GUARD_POINTSCHEMEDRAWER_H__