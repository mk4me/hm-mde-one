#ifndef HEADER_GUARD_GLPOINTSCHEMEDRAWER_H__
#define HEADER_GUARD_GLPOINTSCHEMEDRAWER_H__
#include <map>
#include <osg/Geode>
#include <osg/Vec4f>
#include <plugins/kinematic/SkeletalVisualizationScheme.h>
#include "OsgSchemeDrawer.h"

//! Schemat odrysowywuj�cy punkty
class GlPointSchemeDrawer : public OsgSchemeDrawer
{
public:
    //! Konstruktor
    //! \param sphereComplex z�o�ono�� pojedynczej sfery
    //! \param sphereRadius promie� sfery
    GlPointSchemeDrawer(int sphereComplex, float sphereRadius);
    //! Konstruktor
    //! \param sphereComplex z�o�ono�� sfery
    //! \param sphereRadius promie� sfery
    //! \param color kolor zdefiniowany przez u�ytkownika
    GlPointSchemeDrawer(int sphereComplex, float sphereRadius, const osg::Vec4& color);

public:
    //! Realizacja metody abstrakcyjnej (nieu�ywana)
    virtual void draw();
    //! wywo�ywane, gdy schemat zosta� zmieniony i jego wizualizacja wymaga od�wie�enia
    virtual void update();
    //! Inicjalizacja drawera, tworzone s� w�z�y
    //! \param scheme Dostarczany schemat wizualizacji
    virtual void init(VisualizationSchemeConstPtr scheme);
    //! wywo�ywane przy zako�czeniu dzia�ania, sprz�tni�cie obiekt�w ze sceny
    virtual void deinit();
    //! \return zwraca w�ze� ze stworzonymi po��czeniami
    virtual osg::ref_ptr<osg::Node> getNode() { return node; }


private:
    //! wska�nik do w�z�a przechowywuj�cego geometri�
    typedef osg::ref_ptr<osg::Geode> GeodePtr;
    //! wska�nik do geometrii, kt�ra trafia do w�z�a
    typedef osg::ref_ptr<osg::Geometry> GeometryPtr;
    //! wska�nik do w�z�a zawieraj�cego transformacj�
    typedef osg::ref_ptr<osg::PositionAttitudeTransform> TransformPtr;
    //! aktualny stan markera (kolor, pozycja, ...)
    typedef VisualizationScheme::State MarkerState;

private:
    //! Tworzy wszystkie punkty ze schematu wizualizcji
    //! \param markers kolekcja z markerami
    void createMarkersCrowd(const std::vector<MarkerState>& markers);
    //! Tworzy geode markera (je�li istnieje sfera o danym kolorze to nie tworzy nowej geometrii)
    //! \param color ��dany kolor
    //! \param scale skala sfery
    //! \return wska�nik do stworzonej geody
    GeodePtr createMarker(osg::Vec4 color, float scale = 1.0f);
    //! Tworzy w�ze� markera i ustawia mu odpowiednia transformacj�
    //! \param point punkt w przestrzeni, gdzie ma by� umieszczony w�ze�
    //! \param color ��dany kolor
    //! \param name nazwa w�z�a
    //! \return w�ze� ze stowrzonym markerem
    TransformPtr addPoint(const osg::Vec3& point, const osg::Vec4& color, const std::string& name = std::string());
    //! Tworzy geometri� ze sfera
    //! \param complex z�o�ono�� sfery
    //! \param color kolor sfery
    //! \return wska�nik do stworzonej geometrii
    GeometryPtr createCustomSphere(int complex, osg::Vec4 color) const;
    //! Zmienia kolor, je�li kolor geometrii jest inny niz kolor markera. je�li jest taka potrzeba tworzona jest nowa geometria
    //! \param point w�ze� z markerem
    //! \param state marker pochodz�cy ze schematu wizualizacji
    void changeColorIfNecessary(TransformPtr point, const VisualizationScheme::State& state);

private:
    //! w�ze� ze stworzonymi markerami
    osg::ref_ptr<osg::Group> node;
    //! struktura ze stworzonymi w�z�ami marker�w
    std::vector<TransformPtr> points;
    //! mapa kolor -> geometria, stworzona w celu optymalizacji
    std::map<osg::Vec4f, GeometryPtr> spheresByColor;
    //! domysla z�o�ono�� sfery
    int complex;
    //! domy�lny promie� sfery
    float radius;
    //! domy�lny kolor sfery
    osg::Vec4 customColor;
    //! flaga okre�laj�ca, czy u�ywa� koloru zdefiniowanego przez u�ytkownika
    bool useCustomColor;
};
typedef core::shared_ptr<GlPointSchemeDrawer> GlPointSchemeDrawerPtr;
typedef core::shared_ptr<const GlPointSchemeDrawer> GlPointSchemeDrawerConstPtr;

#endif //HEADER_GUARD_POINTSCHEMEDRAWER_H__
