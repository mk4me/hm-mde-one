/********************************************************************
	created:	2011/09/02
	created:	2:9:2011   9:28
	filename: 	MarkerSerie.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_KINEMATIC__MARKERSERIE_H__
#define HEADER_GUARD_KINEMATIC__MARKERSERIE_H__

#include <corelib/IVisualizer.h>
#include <osg/Geode>
#include "KinematicVisualizer.h"
#include "MarkersVisualizationScheme.h"

//! Seria danych wizualizatora 3D wizualizująca markery z pliku C3D
class MarkerSerie : public QObject, public KinematicSerie
{
	Q_OBJECT;
public:
    //! wskaźnik na węzeł przechowujący geometrię
	typedef osg::ref_ptr<osg::Geode> GeodePtr;
    //! wskaźnik na węzeł przechowujący grupę innych węzłów
	typedef osg::ref_ptr<osg::Group> GroupPtr;

public:
    //! Konstuktor
    //! \param visualizer wizualizator, który stworzył serie danych
	MarkerSerie(KinematicVisualizer * visualizer) : 
	  visualizer(visualizer),
	  markersDrawer(new SchemeDrawerContainer())
	  {
		  
	  }

public:
    //! Ustawia nowa nazwę serii
    virtual void setName(const std::string & name);
    //! \return nazwa serii
    virtual const std::string getName() const;
    //! Ustawienie danych, inicjalizacja 
    //! \param data dane typu JointAnglesCollection
	virtual void setData(const core::TypeInfo & requestedType, const core::ObjectWrapperConstPtr & data);
    //! \return ustawione dane
    virtual const core::ObjectWrapperConstPtr & getData() const;

	virtual void update();

	virtual const utils::TypeInfo & getRequestedDataType() const;
	//! \return długość kanału w sekundach
	virtual double getLength() const;
    //! \return macierz serii z transformacją, która nie została zmieniona przez manipulatory
    virtual osg::Matrix getInitialMatrix() const;
	//! Czas zawiera się między 0 a getLength()
	//! \param time Aktualny, lokalny czas kanału w sekundach
	virtual void setLocalTime(double time);

private slots:
    //! Pokauje/chowa ghosta, który ukazuje charakterystykę ruchu na przestrzeni czasu, tworzy ghosta jeśli jest to konieczne
    //! \param visible czy widoczny
    void showGhost(bool visible);
	
private:
    //! Wizualizator, który utworzył serie
	KinematicVisualizer * visualizer;
    //! Połącznie między markerami z C3D a ich wizualizacją
    MarkersVisualizationSchemePtr scheme;
    //! drawer schematu
	SchemeDrawerContainerPtr markersDrawer;
    //! drawer trajektorii markerów
	TrajectoryDrawerPtr trajectoryDrawer;
    //! węzeł, w którym przechowywany jest ghost
    osg::ref_ptr<osg::PositionAttitudeTransform> ghostNode;
    //! wrapper przekazany serii
    core::ObjectWrapperConstPtr data;

	utils::TypeInfo requestedType;
    //! nazwa serii
    std::string name;
};
typedef boost::shared_ptr<MarkerSerie> MarkerSeriePtr;
typedef boost::shared_ptr<const MarkerSerie> MarkerSerieConstPtr;


#endif
