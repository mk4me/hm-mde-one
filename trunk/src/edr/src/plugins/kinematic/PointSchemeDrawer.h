#ifndef HEADER_GUARD_POINTSCHEMEDRAWER_H__
#define HEADER_GUARD_POINTSCHEMEDRAWER_H__
#include <osg/Geode>
#include <plugins/kinematic/SkeletalVisualizationScheme.h>
#include "OsgSchemeDrawer.h"

//! Schemat odrysowywuj�cy punkty
class PointSchemeDrawer : public OsgSchemeDrawer
{
public:
    //! Konstruktor
    PointSchemeDrawer() {}
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
    GeodePtr createMarker(const osg::Vec4& color, float scale = 1.0f) const;
    //! Tworzy w�ze� markera i ustawia mu odpowiednia transformacj�
    //! \param point punkt w przestrzeni, gdzie ma by� umieszczony w�ze�
    //! \param color ��dany kolor
    //! \param name nazwa w�z�a
    //! \return w�ze� ze stowrzonym markerem
    TransformPtr addPoint(const osg::Vec3& point, const osg::Vec4& color);

private:
    //! w�ze� ze stworzonymi markerami
    osg::ref_ptr<osg::Group> node;
    //! struktura ze stworzonymi w�z�ami marker�w
    std::vector<TransformPtr> points;
};

#endif //HEADER_GUARD_POINTSCHEMEDRAWER_H__
