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

//! Seria danych wizualizatora 3D wizualizuj¹ca markery z pliku C3D
class MarkerSerie : public QObject, public KinematicSerie
{
	Q_OBJECT;
public:
    //! wskaŸnik na wêze³ przechowuj¹cy geometriê
	typedef osg::ref_ptr<osg::Geode> GeodePtr;
    //! wskaŸnik na wêze³ przechowuj¹cy grupê innych wêz³ów
	typedef osg::ref_ptr<osg::Group> GroupPtr;

public:
    //! Konstuktor
    //! \param visualizer wizualizator, który stworzy³ serie danych
	MarkerSerie(KinematicVisualizer * visualizer) : 
	  visualizer(visualizer),
	  markersDrawer(new SchemeDrawerContainer())
	  {
		  
	  }

public:
    //! Ustawia nowa nazwê serii
    virtual void setName(const std::string & name);
    //! \return nazwa serii
    virtual const std::string & getName() const;
    //! Ustawienie danych, inicjalizacja 
    //! \param data dane typu JointAnglesCollection
	virtual void setData(const core::ObjectWrapperConstPtr & data);
    //! \return ustawione dane
    virtual const core::ObjectWrapperConstPtr & getData() const;
	//! \return d³ugoœæ kana³u w sekundach
	virtual double getLength() const;
    //! \return macierz serii z transformacj¹, która nie zosta³a zmieniona przez manipulatory
    virtual osg::Matrix getInitialMatrix() const;
	//! Czas zawiera siê miêdzy 0 a getLength()
	//! \param time Aktualny, lokalny czas kana³u w sekundach
	virtual void setLocalTime(double time);

private slots:
    //! Pokauje/chowa ghosta, który ukazuje charakterystykê ruchu na przestrzeni czasu, tworzy ghosta jeœli jest to konieczne
    //! \param visible czy widoczny
    void showGhost(bool visible);
	
private:
    //! Wizualizator, który utworzy³ serie
	KinematicVisualizer * visualizer;
    //! Po³¹cznie miêdzy markerami z C3D a ich wizualizacj¹
    MarkersVisualizationSchemePtr scheme;
    //! drawer schematu
	SchemeDrawerContainerPtr markersDrawer;
    //! drawer trajektorii markerów
	TrajectoryDrawerPtr trajectoryDrawer;
    //! wêze³, w którym przechowywany jest ghost
    osg::ref_ptr<osg::PositionAttitudeTransform> ghostNode;
    //! wrapper przekazany serii
    core::ObjectWrapperConstPtr data;
    //! nazwa serii
    std::string name;
};
typedef boost::shared_ptr<MarkerSerie> MarkerSeriePtr;
typedef boost::shared_ptr<const MarkerSerie> MarkerSerieConstPtr;


#endif
