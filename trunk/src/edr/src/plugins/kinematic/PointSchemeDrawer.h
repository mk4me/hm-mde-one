#ifndef HEADER_GUARD_POINTSCHEMEDRAWER_H__
#define HEADER_GUARD_POINTSCHEMEDRAWER_H__
#include <osg/Geode>
#include <plugins/kinematic/SkeletalVisualizationScheme.h>
#include "OsgSchemeDrawer.h"

//! Schemat odrysowywuj¹cy punkty
class PointSchemeDrawer : public OsgSchemeDrawer
{
public:
    //! Konstruktor
    PointSchemeDrawer() {}
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
    GeodePtr createMarker(const osg::Vec4& color, float scale = 1.0f) const;
    //! Tworzy wêze³ markera i ustawia mu odpowiednia transformacjê
    //! \param point punkt w przestrzeni, gdzie ma byæ umieszczony wêze³
    //! \param color ¿¹dany kolor
    //! \param name nazwa wêz³a
    //! \return wêze³ ze stowrzonym markerem
    TransformPtr addPoint(const osg::Vec3& point, const osg::Vec4& color);

private:
    //! wêze³ ze stworzonymi markerami
    osg::ref_ptr<osg::Group> node;
    //! struktura ze stworzonymi wêz³ami markerów
    std::vector<TransformPtr> points;
};

#endif //HEADER_GUARD_POINTSCHEMEDRAWER_H__
