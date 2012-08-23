#ifndef HEADER_GUARD_GLPOINTSCHEMEDRAWER_H__
#define HEADER_GUARD_GLPOINTSCHEMEDRAWER_H__
#include <map>
#include <osg/Geode>
#include <osg/Vec4f>
#include <plugins/kinematic/SkeletalVisualizationScheme.h>
#include "OsgSchemeDrawer.h"

//! Schemat odrysowywuj¹cy punkty
class GlPointSchemeDrawer : public OsgSchemeDrawer
{
public:
    //! Konstruktor
    //! \param sphereComplex z³o¿onoœæ pojedynczej sfery
    //! \param sphereRadius promieñ sfery
    GlPointSchemeDrawer(int sphereComplex, float sphereRadius);
    //! Konstruktor
    //! \param sphereComplex z³o¿onoœæ sfery
    //! \param sphereRadius promieñ sfery
    //! \param color kolor zdefiniowany przez u¿ytkownika
    GlPointSchemeDrawer(int sphereComplex, float sphereRadius, const osg::Vec4& color);

public:
    //! Realizacja metody abstrakcyjnej (nieu¿ywana)
    virtual void draw();
    //! wywo³ywane, gdy schemat zosta³ zmieniony i jego wizualizacja wymaga odœwie¿enia
    virtual void update();
    //! Inicjalizacja drawera, tworzone s¹ wêz³y
    //! \param scheme Dostarczany schemat wizualizacji
    virtual void init(VisualizationSchemeConstPtr scheme);
    //! wywo³ywane przy zakoñczeniu dzia³ania, sprz¹tniêcie obiektów ze sceny
    virtual void deinit();
    //! \return zwraca wêze³ ze stworzonymi po³¹czeniami
    virtual osg::ref_ptr<osg::Node> getNode() { return node; }


private:
    //! wskaŸnik do wêz³a przechowywuj¹cego geometriê
    typedef osg::ref_ptr<osg::Geode> GeodePtr;
    //! wskaŸnik do geometrii, która trafia do wêz³a
    typedef osg::ref_ptr<osg::Geometry> GeometryPtr;
    //! wskaŸnik do wêz³a zawieraj¹cego transformacjê
    typedef osg::ref_ptr<osg::PositionAttitudeTransform> TransformPtr;
    //! aktualny stan markera (kolor, pozycja, ...)
    typedef VisualizationScheme::State MarkerState;

private:
    //! Tworzy wszystkie punkty ze schematu wizualizcji
    //! \param markers kolekcja z markerami
    void createMarkersCrowd(const std::vector<MarkerState>& markers);
    //! Tworzy geode markera (jeœli istnieje sfera o danym kolorze to nie tworzy nowej geometrii)
    //! \param color ¿¹dany kolor
    //! \param scale skala sfery
    //! \return wskaŸnik do stworzonej geody
    GeodePtr createMarker(osg::Vec4 color, float scale = 1.0f);
    //! Tworzy wêze³ markera i ustawia mu odpowiednia transformacjê
    //! \param point punkt w przestrzeni, gdzie ma byæ umieszczony wêze³
    //! \param color ¿¹dany kolor
    //! \param name nazwa wêz³a
    //! \return wêze³ ze stowrzonym markerem
    TransformPtr addPoint(const osg::Vec3& point, const osg::Vec4& color, const std::string& name = std::string());
    //! Tworzy geometriê ze sfera
    //! \param complex z³o¿onoœæ sfery
    //! \param color kolor sfery
    //! \return wskaŸnik do stworzonej geometrii
    GeometryPtr createCustomSphere(int complex, osg::Vec4 color) const;
    //! Zmienia kolor, jeœli kolor geometrii jest inny niz kolor markera. jeœli jest taka potrzeba tworzona jest nowa geometria
    //! \param point wêze³ z markerem
    //! \param state marker pochodz¹cy ze schematu wizualizacji
    void changeColorIfNecessary(TransformPtr point, const VisualizationScheme::State& state);

private:
    //! wêze³ ze stworzonymi markerami
    osg::ref_ptr<osg::Group> node;
    //! struktura ze stworzonymi wêz³ami markerów
    std::vector<TransformPtr> points;
    //! mapa kolor -> geometria, stworzona w celu optymalizacji
    std::map<osg::Vec4f, GeometryPtr> spheresByColor;
    //! domysla z³o¿onoœæ sfery
    int complex;
    //! domyœlny promieñ sfery
    float radius;
    //! domyœlny kolor sfery
    osg::Vec4 customColor;
    //! flaga okreœlaj¹ca, czy u¿ywaæ koloru zdefiniowanego przez u¿ytkownika
    bool useCustomColor;
};
typedef core::shared_ptr<GlPointSchemeDrawer> GlPointSchemeDrawerPtr;
typedef core::shared_ptr<const GlPointSchemeDrawer> GlPointSchemeDrawerConstPtr;

#endif //HEADER_GUARD_POINTSCHEMEDRAWER_H__
