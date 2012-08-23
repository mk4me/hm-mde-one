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

//! Klasa s³u¿y do odrysowania trajektorii markerów
class TrajectoryDrawer : public OsgSchemeDrawer
{
public:
	//! Konstruktor
	//! \param color domyœlny kolor trajektorii
	//! \param density z³o¿onoœæ trajektorii (liczba wierzcho³ków j¹ tworz¹ca)
	TrajectoryDrawer(const osg::Vec4& color, int density);

public:
	//! \return wêze³ z trajektoriami
	virtual osg::ref_ptr<osg::Node> getNode();
    //! Realizacja metody abstrakcyjnej (nieu¿ywana)
    virtual void draw();
    //! wywo³ywane, gdy schemat zosta³ zmieniony i jego wizualizacja wymaga odœwie¿enia
    virtual void update();
    //! Inicjalizacja drawera, tworzone s¹ wêz³y
    //! \param scheme Dostarczany schemat wizualizacji
    virtual void init(VisualizationSchemeConstPtr scheme);
    //! wywo³ywane przy zakoñczeniu dzia³ania, sprz¹tniêcie obiektów ze sceny
    virtual void deinit();
	//! Zmienia widocznoœæ trajekorii
	//! \param name nazwa markera, dla którego stworzona zosta³a zmieniana trajektoria
	//! \param visible czy trajektoria ma byæ widoczna?
	void setVisible(const std::string& name, bool visible);
	//! pobiera kolor trajektorii
	//! \param name nazwa markera, dla którego stworzona zosta³a trajektoria
    //! \return
	osg::Vec4 getColor(const std::string& name);
	//! ustawia kolor trajektorii
	//! \param name nazwa markera, dla którego stworzona zosta³a trajektoria
	//! \param color nowy kolor trajektorii
	void setColor(const std::string& name, const osg::Vec4& color);
	//! Zmienia gruboœæ linii trajektorii
	//! \param name nazwa markera, dla którego stworzona zosta³a zmieniana trajektoria
	//! \param width nowa szerokoœæ trajektorii
	void setLineWidth(const std::string& name, float width);
	//! pobiera szerokoœæ trajektorii
	//! \param name nazwa markera, dla którego stworzona zosta³a trajektoria
	float getLineWidth(const std::string& name) const;
	//! Zwraca przedzia³ czasowy, w którym ma byæ widoczna trajektoria
	//! \param name nazwa markera, dla którego stworzona zosta³a trajektoria
    //! \return czas w formie od  do
	std::pair<float, float> getTimes(const std::string& name) const;
	//! Ustawia przedzia³ czasowy, w którym ma byæ widoczna trajektoria
	//! \param name nazwa markera, dla którego stworzona zosta³a zmieniana trajektoria
	//! \param times czas w formie od  do
	void setTimes(const std::string& name, const std::pair<float, float>& times);
    //! \return markery, dla których zosta³y utworzone trajektorie
    MarkerCollectionConstPtr getMarkers() const;
    //! Metoda pozwala zmienic wzgledna pozycje wêz³a
    //! \param offset nowe przesuniecie wêz³a z trajektoriami
    void setOffset(const osg::Vec3& offset);

private:
	//! Metoda tworzy wszystkie trajektorie
	//! \param markers kolekcje markerów, na podstawie których stworzone zostan¹ trajektorie
	void createTrajectories(MarkerCollectionConstPtr markers);

private:
     //! wêze³, do którego podpiête zostan¹ trajektorie
	 osg::ref_ptr<osg::PositionAttitudeTransform> node;
     //! gestosc trajektori (liczba wierzcho³ków j¹ tworz¹cych)
	 int density;
     //! domyœlny kolor trajektorii
	 osg::Vec4 color;
	 //! mapa nazwa markera -> geometria z trajektoria
	 std::map<std::string, GeodePtr> trajectoriesMap;
     //! mapa nazwa markera -> przedzia³ czasowy trajektorii
	 std::map<std::string, std::pair<float, float> > timesMap;
     //! mapa nazwa markera -> gruboœæ lini trajektorii
	 std::map<std::string, float> thicknessMap;
};
typedef boost::shared_ptr<TrajectoryDrawer> TrajectoryDrawerPtr;
typedef boost::shared_ptr<const TrajectoryDrawer> TrajectoryDrawerConstPtr;

#endif
