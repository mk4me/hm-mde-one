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

//! Klasa s�u�y do odrysowania trajektorii marker�w
class TrajectoryDrawer : public OsgSchemeDrawer
{
public:
	//! Konstruktor
	//! \param color domy�lny kolor trajektorii
	//! \param density z�o�ono�� trajektorii (liczba wierzcho�k�w j� tworz�ca)
	TrajectoryDrawer(const osg::Vec4& color, int density);

public:
	//! \return w�ze� z trajektoriami
	virtual osg::ref_ptr<osg::Node> getNode();
    //! Realizacja metody abstrakcyjnej (nieu�ywana)
    virtual void draw();
    //! wywo�ywane, gdy schemat zosta� zmieniony i jego wizualizacja wymaga od�wie�enia
    virtual void update();
    //! Inicjalizacja drawera, tworzone s� w�z�y
    //! \param scheme Dostarczany schemat wizualizacji
    virtual void init(VisualizationSchemeConstPtr scheme);
    //! wywo�ywane przy zako�czeniu dzia�ania, sprz�tni�cie obiekt�w ze sceny
    virtual void deinit();
	//! Zmienia widoczno�� trajekorii
	//! \param name nazwa markera, dla kt�rego stworzona zosta�a zmieniana trajektoria
	//! \param visible czy trajektoria ma by� widoczna?
	void setVisible(const std::string& name, bool visible);
	//! pobiera kolor trajektorii
	//! \param name nazwa markera, dla kt�rego stworzona zosta�a trajektoria
    //! \return
	osg::Vec4 getColor(const std::string& name);
	//! ustawia kolor trajektorii
	//! \param name nazwa markera, dla kt�rego stworzona zosta�a trajektoria
	//! \param color nowy kolor trajektorii
	void setColor(const std::string& name, const osg::Vec4& color);
	//! Zmienia grubo�� linii trajektorii
	//! \param name nazwa markera, dla kt�rego stworzona zosta�a zmieniana trajektoria
	//! \param width nowa szeroko�� trajektorii
	void setLineWidth(const std::string& name, float width);
	//! pobiera szeroko�� trajektorii
	//! \param name nazwa markera, dla kt�rego stworzona zosta�a trajektoria
	float getLineWidth(const std::string& name) const;
	//! Zwraca przedzia� czasowy, w kt�rym ma by� widoczna trajektoria
	//! \param name nazwa markera, dla kt�rego stworzona zosta�a trajektoria
    //! \return czas w formie od  do
	std::pair<float, float> getTimes(const std::string& name) const;
	//! Ustawia przedzia� czasowy, w kt�rym ma by� widoczna trajektoria
	//! \param name nazwa markera, dla kt�rego stworzona zosta�a zmieniana trajektoria
	//! \param times czas w formie od  do
	void setTimes(const std::string& name, const std::pair<float, float>& times);
    //! \return markery, dla kt�rych zosta�y utworzone trajektorie
    MarkerCollectionConstPtr getMarkers() const;
    //! Metoda pozwala zmienic wzgledna pozycje w�z�a
    //! \param offset nowe przesuniecie w�z�a z trajektoriami
    void setOffset(const osg::Vec3& offset);

private:
	//! Metoda tworzy wszystkie trajektorie
	//! \param markers kolekcje marker�w, na podstawie kt�rych stworzone zostan� trajektorie
	void createTrajectories(MarkerCollectionConstPtr markers);

private:
     //! w�ze�, do kt�rego podpi�te zostan� trajektorie
	 osg::ref_ptr<osg::PositionAttitudeTransform> node;
     //! gestosc trajektori (liczba wierzcho�k�w j� tworz�cych)
	 int density;
     //! domy�lny kolor trajektorii
	 osg::Vec4 color;
	 //! mapa nazwa markera -> geometria z trajektoria
	 std::map<std::string, GeodePtr> trajectoriesMap;
     //! mapa nazwa markera -> przedzia� czasowy trajektorii
	 std::map<std::string, std::pair<float, float> > timesMap;
     //! mapa nazwa markera -> grubo�� lini trajektorii
	 std::map<std::string, float> thicknessMap;
};
typedef boost::shared_ptr<TrajectoryDrawer> TrajectoryDrawerPtr;
typedef boost::shared_ptr<const TrajectoryDrawer> TrajectoryDrawerConstPtr;

#endif
