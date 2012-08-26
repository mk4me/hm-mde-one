/********************************************************************
	created:	2011/09/06
	created:	6:9:2011   13:34
	filename: 	TrajectoriesDrawer.h
	author:		Wojciech Kniec

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_TRAJECTORIES_DRAWER__TRAJECTORIESDRAWER_H__
#define HEADER_GUARD_TRAJECTORIES_DRAWER__TRAJECTORIESDRAWER_H__

#include <osg/Geode>
#include <plugins/kinematic/SkeletalVisualizationScheme.h>
#include <plugins/c3d/C3DChannels.h>
#include "OsgSchemeDrawer.h"

//! Klasa służy do odrysowania trajektorii markerów
class TrajectoryDrawer : public OsgSchemeDrawer
{
public:
	//! Konstruktor
	//! \param color domyślny kolor trajektorii
	//! \param density złożoność trajektorii (liczba wierzchołków ją tworząca)
	TrajectoryDrawer(const osg::Vec4& color, int density);

public:
	//! \return węzeł z trajektoriami
	virtual osg::ref_ptr<osg::Node> getNode();
    //! Realizacja metody abstrakcyjnej (nieużywana)
    virtual void draw();
    //! wywoływane, gdy schemat został zmieniony i jego wizualizacja wymaga odświeżenia
    virtual void update();
    //! Inicjalizacja drawera, tworzone są węzły
    //! \param scheme Dostarczany schemat wizualizacji
    virtual void init(VisualizationSchemeConstPtr scheme);
    //! wywoływane przy zakończeniu działania, sprzątnięcie obiektów ze sceny
    virtual void deinit();
	//! Zmienia widoczność trajekorii
	//! \param name nazwa markera, dla którego stworzona została zmieniana trajektoria
	//! \param visible czy trajektoria ma być widoczna?
	void setVisible(const std::string& name, bool visible);
	//! pobiera kolor trajektorii
	//! \param name nazwa markera, dla którego stworzona została trajektoria
    //! \return
	osg::Vec4 getColor(const std::string& name);
	//! ustawia kolor trajektorii
	//! \param name nazwa markera, dla którego stworzona została trajektoria
	//! \param color nowy kolor trajektorii
	void setColor(const std::string& name, const osg::Vec4& color);
	//! Zmienia grubość linii trajektorii
	//! \param name nazwa markera, dla którego stworzona została zmieniana trajektoria
	//! \param width nowa szerokość trajektorii
	void setLineWidth(const std::string& name, float width);
	//! pobiera szerokość trajektorii
	//! \param name nazwa markera, dla którego stworzona została trajektoria
	float getLineWidth(const std::string& name) const;
	//! Zwraca przedział czasowy, w którym ma być widoczna trajektoria
	//! \param name nazwa markera, dla którego stworzona została trajektoria
    //! \return czas w formie od  do
	std::pair<float, float> getTimes(const std::string& name) const;
	//! Ustawia przedział czasowy, w którym ma być widoczna trajektoria
	//! \param name nazwa markera, dla którego stworzona została zmieniana trajektoria
	//! \param times czas w formie od  do
	void setTimes(const std::string& name, const std::pair<float, float>& times);
    //! \return markery, dla których zostały utworzone trajektorie
    MarkerCollectionConstPtr getMarkers() const;
    //! Metoda pozwala zmienic względna pozycje węzła
    //! \param offset nowe przesunięcie węzła z trajektoriami
    void setOffset(const osg::Vec3& offset);

private:
	//! Metoda tworzy wszystkie trajektorie
	//! \param markers kolekcje markerów, na podstawie których stworzone zostaną trajektorie
	void createTrajectories(MarkerCollectionConstPtr markers);

private:
     //! węzeł, do którego podpięte zostaną trajektorie
	 osg::ref_ptr<osg::PositionAttitudeTransform> node;
     //! gestość trajektori (liczba wierzchołków ją tworzących)
	 int density;
     //! domyślny kolor trajektorii
	 osg::Vec4 color;
	 //! mapa nazwa markera -> geometria z trajektoria
	 std::map<std::string, GeodePtr> trajectoriesMap;
     //! mapa nazwa markera -> przedział czasowy trajektorii
	 std::map<std::string, std::pair<float, float> > timesMap;
     //! mapa nazwa markera -> grubość lini trajektorii
	 std::map<std::string, float> thicknessMap;
};
typedef boost::shared_ptr<TrajectoryDrawer> TrajectoryDrawerPtr;
typedef boost::shared_ptr<const TrajectoryDrawer> TrajectoryDrawerConstPtr;

#endif
