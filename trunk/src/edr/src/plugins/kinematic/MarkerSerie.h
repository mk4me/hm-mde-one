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

//! Seria danych wizualizatora 3D wizualizuj�ca markery z pliku C3D
class MarkerSerie : public QObject, public KinematicSerie
{
	Q_OBJECT;
public:
    //! wska�nik na w�ze� przechowuj�cy geometri�
	typedef osg::ref_ptr<osg::Geode> GeodePtr;
    //! wska�nik na w�ze� przechowuj�cy grup� innych w�z��w
	typedef osg::ref_ptr<osg::Group> GroupPtr;

public:
    //! Konstuktor
    //! \param visualizer wizualizator, kt�ry stworzy� serie danych
	MarkerSerie(KinematicVisualizer * visualizer) : 
	  visualizer(visualizer),
	  markersDrawer(new SchemeDrawerContainer())
	  {
		  
	  }

public:
    //! Ustawia nowa nazw� serii
    virtual void setName(const std::string & name);
    //! \return nazwa serii
    virtual const std::string & getName() const;
    //! Ustawienie danych, inicjalizacja 
    //! \param data dane typu JointAnglesCollection
	virtual void setData(const core::ObjectWrapperConstPtr & data);
    //! \return ustawione dane
    virtual const core::ObjectWrapperConstPtr & getData() const;
	//! \return d�ugo�� kana�u w sekundach
	virtual double getLength() const;
    //! \return macierz serii z transformacj�, kt�ra nie zosta�a zmieniona przez manipulatory
    virtual osg::Matrix getInitialMatrix() const;
	//! Czas zawiera si� mi�dzy 0 a getLength()
	//! \param time Aktualny, lokalny czas kana�u w sekundach
	virtual void setLocalTime(double time);

private slots:
    //! Pokauje/chowa ghosta, kt�ry ukazuje charakterystyk� ruchu na przestrzeni czasu, tworzy ghosta je�li jest to konieczne
    //! \param visible czy widoczny
    void showGhost(bool visible);
	
private:
    //! Wizualizator, kt�ry utworzy� serie
	KinematicVisualizer * visualizer;
    //! Po��cznie mi�dzy markerami z C3D a ich wizualizacj�
    MarkersVisualizationSchemePtr scheme;
    //! drawer schematu
	SchemeDrawerContainerPtr markersDrawer;
    //! drawer trajektorii marker�w
	TrajectoryDrawerPtr trajectoryDrawer;
    //! w�ze�, w kt�rym przechowywany jest ghost
    osg::ref_ptr<osg::PositionAttitudeTransform> ghostNode;
    //! wrapper przekazany serii
    core::ObjectWrapperConstPtr data;
    //! nazwa serii
    std::string name;
};
typedef boost::shared_ptr<MarkerSerie> MarkerSeriePtr;
typedef boost::shared_ptr<const MarkerSerie> MarkerSerieConstPtr;


#endif
