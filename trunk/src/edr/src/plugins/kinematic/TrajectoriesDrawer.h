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
#include <plugins/kinematic/skeletalVisualizationScheme.h>
#include <plugins/c3d/C3DChannels.h>
#include "OsgSchemeDrawer.h"

//! Klasa sluzy do odrysowania krajektori markerow
class TrajectoryDrawer : public OsgSchemeDrawer
{
public:
	//! Konstruktor
	//! \param color domyslny kolor trajektorii
	//! \param density zlozonosc trajektorii (liczba wierzcholkow ja tworzaca)
	TrajectoryDrawer(const osg::Vec4& color, int density);

public:
	//! \return wezel z trajektoriami
	virtual osg::ref_ptr<osg::Node> getNode();
    //! Realizacja metody abstrakcyjnej (nieuzywana)
    virtual void draw();
    //! Wywolywane, gdy schemat zostal zmieniony i jego wizualizacja wymaga odswiezenia
    virtual void update();
    //! Inicjalizacja drawera, tworzone sa wezly
    //! \param scheme Dostarczany schemat wizualizacji
    virtual void init(VisualizationSchemeConstPtr scheme);
    //! Wywolywane przy zakonczeniu dzialania, sprzatniecie obiektow ze sceny
    virtual void deinit();
	//! Zmienia widocznosc trajekorii
	//! \param name nazwa markera, dla ktorego stworzona zostala zmieniana trajektoria
	//! \param visible czy trajektoria ma byc widoczna?
	void setVisible(const std::string& name, bool visible);
	//! pobiera kolor trajektorii
	//! \param name nazwa markera, dla ktorego stworzona zostala trajektoria
    //! \return 
	osg::Vec4 getColor(const std::string& name);
	//! ustawia kolor trajektorii
	//! \param name nazwa markera, dla ktorego stworzona zostala trajektoria
	//! \param color nowy kolor trajektorii
	void setColor(const std::string& name, const osg::Vec4& color);
	//! Zmienia grubosc linii trajektorii
	//! \param name nazwa markera, dla ktorego stworzona zostala zmieniana trajektoria
	//! \param width nowa szerokosc trajektorii
	void setLineWidth(const std::string& name, float width);
	//! pobiera szerokosc trajektorii
	//! \param name nazwa markera, dla ktorego stworzona zostala trajektoria
	float getLineWidth(const std::string& name) const;
	//! Zwraca przedzial czasowy, w ktorym ma byc widoczna trajektoria
	//! \param name nazwa markera, dla ktorego stworzona zostala trajektoria
    //! \return czas w formie od  do
	std::pair<float, float> getTimes(const std::string& name) const;
	//! Ustawia przedzial czasowy, w ktorym ma byc widoczna trajektoria
	//! \param name nazwa markera, dla ktorego stworzona zostala zmieniana trajektoria
	//! \param times czas w formie od  do
	void setTimes(const std::string& name, const std::pair<float, float>& times);
    //! \return markery, dla ktorych zostaly utworzone trajektorie
    MarkerCollectionConstPtr getMarkers() const;
    //! Metoda pozwala zmienic wzgledna pozycje wezla
    //! \param offset nowe przesuniecie wezla z trajektoriami
    void setOffset(const osg::Vec3& offset);
	
private:
	//! Metoda tworzy wszystkie trajektorie
	//! \param markers kolekcje markerow, na podstawie ktorych stworzone zostana trajektorie
	void createTrajectories(MarkerCollectionConstPtr markers);

private:
     //! wezel, do ktorego podpiete zostana trajektorie
	 osg::ref_ptr<osg::PositionAttitudeTransform> node;
     //! gestosc trajektori (liczba wierzcholkow ja tworzacych)
	 int density;
     //! domyslny kolor trajektorii
	 osg::Vec4 color;
	 //! mapa nazwa markera -> geometria z trajektoria
	 std::map<std::string, GeodePtr> trajectoriesMap;
     //! mapa nazwa markera -> przedzial czasowy trajektorii
	 std::map<std::string, std::pair<float, float> > timesMap;
     //! mapa nazwa markera -> grubosc lini trajektorii
	 std::map<std::string, float> thicknessMap;
};
typedef boost::shared_ptr<TrajectoryDrawer> TrajectoryDrawerPtr;
typedef boost::shared_ptr<const TrajectoryDrawer> TrajectoryDrawerConstPtr;

#endif