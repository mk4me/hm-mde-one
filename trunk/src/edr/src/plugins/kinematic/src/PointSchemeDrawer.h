#ifndef HEADER_GUARD_POINTSCHEMEDRAWER_H__
#define HEADER_GUARD_POINTSCHEMEDRAWER_H__
#include <osg/Geode>
#include "SkeletalVisualizationScheme.h"
#include "OsgSchemeDrawer.h"

//! Schemat odrysowywujący punkty
class PointSchemeDrawer : public OsgSchemeDrawer
{
public:
    //! Konstruktor
    PointSchemeDrawer() {}
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
    GeodePtr createMarker(const osg::Vec4& color, float scale = 1.0f) const;
    //! Tworzy węzeł markera i ustawia mu odpowiednia transformację
    //! \param point punkt w przestrzeni, gdzie ma być umieszczony węzeł
    //! \param color żądany kolor
    //! \param name nazwa węzła
    //! \return węzeł ze stowrzonym markerem
    TransformPtr addPoint(const osg::Vec3& point, const osg::Vec4& color);

private:
    //! węzeł ze stworzonymi markerami
    osg::ref_ptr<osg::Group> node;
    //! struktura ze stworzonymi węzłami markerów
    std::vector<TransformPtr> points;
};

#endif //HEADER_GUARD_POINTSCHEMEDRAWER_H__
