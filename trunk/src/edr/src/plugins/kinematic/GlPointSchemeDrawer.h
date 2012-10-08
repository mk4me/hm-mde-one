#ifndef HEADER_GUARD_GLPOINTSCHEMEDRAWER_H__
#define HEADER_GUARD_GLPOINTSCHEMEDRAWER_H__
#include <map>
#include <osg/Geode>
#include <osg/Vec4f>
#include <plugins/kinematic/SkeletalVisualizationScheme.h>
#include "OsgSchemeDrawer.h"

//! Schemat odrysowywujący punkty
class GlPointSchemeDrawer : public OsgSchemeDrawer
{
public:
    //! Konstruktor
    //! \param sphereComplex złożoność pojedynczej sfery
    //! \param sphereRadius promień sfery
    GlPointSchemeDrawer(int sphereComplex, float sphereRadius);
    //! Konstruktor
    //! \param sphereComplex złożoność sfery
    //! \param sphereRadius promień sfery
    //! \param color kolor zdefiniowany przez użytkownika
    GlPointSchemeDrawer(int sphereComplex, float sphereRadius, const osg::Vec4& color);

public:
    //! Realizacja metody abstrakcyjnej (nieużywana)
    virtual void draw();
    //! wywoływane, gdy schemat został zmieniony i jego wizualizacja wymaga odświeżenia
    virtual void update();
    //! Inicjalizacja drawera, tworzone są węzły
    //! \param scheme Dostarczany schemat wizualizacji
    virtual void init(VisualizationSchemeConstPtr scheme);
    //! wywoływane przy zakończeniu działania, sprzątnięcie obiektów ze sceny
    virtual void deinit();
    //! \return zwraca węzeł ze stworzonymi połączeniami
    virtual osg::ref_ptr<osg::Node> getNode() { return node; }


private:
    //! wskaźnik do węzła przechowywującego geometrię
    typedef osg::ref_ptr<osg::Geode> GeodePtr;
    //! wskaźnik do geometrii, która trafia do węzła
    typedef osg::ref_ptr<osg::Geometry> GeometryPtr;
    //! wskaźnik do węzła zawierającego transformację
    typedef osg::ref_ptr<osg::PositionAttitudeTransform> TransformPtr;
    //! aktualny stan markera (kolor, pozycja, ...)
    typedef VisualizationScheme::State MarkerState;

private:
    //! Tworzy wszystkie punkty ze schematu wizualizcji
    //! \param markers kolekcja z markerami
    void createMarkersCrowd(const std::vector<MarkerState>& markers);
    //! Tworzy geode markera (jeśli istnieje sfera o danym kolorze to nie tworzy nowej geometrii)
    //! \param color żądany kolor
    //! \param scale skala sfery
    //! \return wskaźnik do stworzonej geody
    GeodePtr createMarker(osg::Vec4 color, float scale = 1.0f);
    //! Tworzy węzeł markera i ustawia mu odpowiednia transformację
    //! \param point punkt w przestrzeni, gdzie ma być umieszczony węzeł
    //! \param color żądany kolor
    //! \param name nazwa węzła
    //! \return węzeł ze stowrzonym markerem
    TransformPtr addPoint(const osg::Vec3& point, const osg::Vec4& color, const std::string& name = std::string());
    //! Tworzy geometrię ze sfera
    //! \param complex złożoność sfery
    //! \param color kolor sfery
    //! \return wskaźnik do stworzonej geometrii
    GeometryPtr createCustomSphere(int complex, osg::Vec4 color) const;
    //! Zmienia kolor, jeśli kolor geometrii jest inny niz kolor markera. jeśli jest taka potrzeba tworzona jest nowa geometria
    //! \param point węzeł z markerem
    //! \param state marker pochodzący ze schematu wizualizacji
    void changeColorIfNecessary(TransformPtr point, const VisualizationScheme::State& state);

private:
    //! węzeł ze stworzonymi markerami
    osg::ref_ptr<osg::Group> node;
    //! struktura ze stworzonymi węzłami markerów
    std::vector<TransformPtr> points;
    //! mapa kolor -> geometria, stworzona w celu optymalizacji
    std::map<osg::Vec4f, GeometryPtr> spheresByColor;
    //! domyślna złożoność sfery
    int complex;
    //! domyślny promień sfery
    float radius;
    //! domyślny kolor sfery
    osg::Vec4 customColor;
    //! flaga określająca, czy używać koloru zdefiniowanego przez użytkownika
    bool useCustomColor;
};
typedef core::shared_ptr<GlPointSchemeDrawer> GlPointSchemeDrawerPtr;
typedef core::shared_ptr<const GlPointSchemeDrawer> GlPointSchemeDrawerConstPtr;

#endif //HEADER_GUARD_POINTSCHEMEDRAWER_H__
