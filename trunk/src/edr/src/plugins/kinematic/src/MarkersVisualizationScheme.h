/********************************************************************
	created:	2012/04/23
	created:	23:4:2012   12:47
	filename: 	MarkersVisualizationScheme.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_KINEMATIC__MARKERSVISUALIZATIONSCHEME_H__
#define HEADER_GUARD_KINEMATIC__MARKERSVISUALIZATIONSCHEME_H__

#include "VisualizationScheme.h"

//! połączenie między kolekcją markerów a ich wizualizacją
class MarkersVisualizationScheme : public VisualizationScheme
{
public:
    //! \return kolekcja z ustawionymi markerami
    MarkerCollectionConstPtr getMarkers() const;
    //! \return czas trwania animacji
    virtual double getDuration() const;
    //! \return czy poprawne dane zostały dostarczone?
    virtual bool hasData() const;
    //! \return środek geometryczny
    virtual osg::Vec3 getCurrentPosition() const;
    //! \return środek geometryczny
    //! \param time czas, dla którego zwracamy środek geometryczny
    osg::Vec3 getRootPosition(double time) const;
    //! ustawienie kolekcji z markerami
    //! \param val obiekt z markerami
    void setMarkers(MarkerCollectionConstPtr val);
    //! ustawienie połączeń i ich kolorów
    //! \param vsk parser z wczytanymi danymi nt. połączeń markerów
    void setMarkersDataFromVsk(kinematic::VskParserConstPtr vsk);

protected:
    //! aktualizacja schematu
    //! \param time czas, dla którego dokonywana jest aktualizacja
    virtual void update( double time );

private:
    //! odświeżenie informacji o markerach
    //! \param time czas, dla którego dokonywana jest aktualizacja
    void updateMarkers(double time);
    //! Oblicza typowa długość pomiędzy dwoma markerami, odległości liczone są 100 razy dla animacji, tworzony jest histogram
    //! \param channel1 pierwszy marker
    //! \param channel2 drugi marker
    //! \param epsilon przedział histogramu
    //! \return typowa długość między markerami
    float getStickLength( VectorChannelConstPtr channel1, VectorChannelConstPtr channel2, float epsilon = 0.001f );

private:
    //! kolekcja z markerami
    MarkerCollectionConstPtr markers;
    //! aktualny środek geometryczny
    osg::Vec3 currentPosition;
};
typedef core::shared_ptr<MarkersVisualizationScheme> MarkersVisualizationSchemePtr;
typedef core::shared_ptr<const MarkersVisualizationScheme> MarkersVisualizationSchemeConstPtr;

#endif
