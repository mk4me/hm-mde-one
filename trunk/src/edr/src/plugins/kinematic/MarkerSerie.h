/********************************************************************
	created:	2011/09/02
	created:	2:9:2011   9:28
	filename: 	MarkerSerie.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_KINEMATIC__MARKERSERIE_H__
#define HEADER_GUARD_KINEMATIC__MARKERSERIE_H__

#include <core/IVisualizer.h>
#include <osg/Geode>
#include "KinematicVisualizer.h"
#include "MarkersVisualizationScheme.h"

//! Seria danych wizualizatora 3D wizualizujaca markery z pliku C3D
class MarkerSerie : public QObject, public KinematicSerie
{
	Q_OBJECT;
public:
	typedef osg::ref_ptr<osg::Geode> GeodePtr;
	typedef osg::ref_ptr<osg::Group> GroupPtr;

public:
    //! Konstuktor
    //! \param visualizer wizualizator, ktory stworzyl serie danych
	MarkerSerie(KinematicVisualizer * visualizer) : 
	  visualizer(visualizer),
	  markersDrawer(new SchemeDrawerContainer())
	  {
		  
	  }

public:
    //! Ustawia nowa nazwe seri
    //! \param name 
    virtual void setName(const std::string & name);
    //! \return nazwa serii
    virtual const std::string & getName() const;
    //! Ustawienie danych, inicjalizacja 
    //! \param data dane typu JointAnglesCollection
	virtual void setData(const core::ObjectWrapperConstPtr & data);
    //! \return ustawione dane
    virtual const core::ObjectWrapperConstPtr & getData() const;
	//! \return Dlugosc kanalu w sekundach
	virtual double getLength() const;
    //! \return macierz serii z transformacja, ktora nie zostala zmieniona przez manipulatory
    virtual osg::Matrix getInitialMatrix() const;
	//! Czas zawiera siê miêdzy 0 a getLength()
	//! \param time Aktualny, lokalny czas kanalu w sekundach
	virtual void setLocalTime(double time);

private slots:
    //! Pokauje/chowa ghosta, ktory ukazuje charakterystyke ruchu na przestrzeni czasu, tworzy ghosta jesli jest to konieczne
    //! \param visible czy widoczny
    void showGhost(bool visible);
	
private:
    //! Wizualizator, ktory utworzyl serie
	KinematicVisualizer * visualizer;
    //! Polacznie miedzy markerami z C3D a ich wizualizacja
    MarkersVisualizationSchemePtr scheme;
    //! drawer schematu
	SchemeDrawerContainerPtr markersDrawer;
    //! drawer trajektorii markerow
	TrajectoryDrawerPtr trajectoryDrawer;
    //! wezel, w ktorym przechowywany jest ghost
    osg::ref_ptr<osg::PositionAttitudeTransform> ghostNode;
    //! wrapper przekazany serii
    core::ObjectWrapperConstPtr data;
    //! nazwa serii
    std::string name;
};
typedef boost::shared_ptr<MarkerSerie> MarkerSeriePtr;
typedef boost::shared_ptr<const MarkerSerie> MarkerSerieConstPtr;


#endif
