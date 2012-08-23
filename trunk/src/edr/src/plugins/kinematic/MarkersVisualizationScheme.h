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

//! po��czenie mi�dzy kolekcj� marker�w a ich wizualizacj�
class MarkersVisualizationScheme : public VisualizationScheme
{
public:
    //! \return kolekcja z ustawionymi markerami
    MarkerCollectionConstPtr getMarkers() const;
    //! \return czas trwania animacji
    virtual double getDuration() const;
    //! \return czy poprawne dane zosta�y dostarczone?
    virtual bool hasData() const;
    //! \return �rodek geometryczny
    virtual osg::Vec3 getCurrentPosition() const;
    //! \return �rodek geometryczny
    //! \param time czas, dla kt�rego zwracamy �rodek geometryczny
    osg::Vec3 getRootPosition(double time) const;
    //! ustawienie kolekcji z markerami
    //! \param val obiekt z markerami
    void setMarkers(MarkerCollectionConstPtr val);
    //! ustawienie po��cze� i ich kolorow
    //! \param vsk parser z wczytanymi danymi nt. po��cze� marker�w
    void setMarkersDataFromVsk(kinematic::VskParserConstPtr vsk);

protected:
    //! aktualizacja schematu
    //! \param time czas, dla kt�rego dokonywana jest aktualizacja
    virtual void update( double time );

private:
    //! od�wie�enie informacji o markerach
    //! \param time czas, dla kt�rego dokonywana jest aktualizacja
    void updateMarkers(double time);
    //! Oblicza typowa d�ugo�� pomi�dzy dwoma markerami, odleg�o�ci liczone s� 100 razy dla animacji, tworzony jest histogram
    //! \param channel1 pierwszy marker
    //! \param channel2 drugi marker
    //! \param epsilon przedzia� histogramu
    //! \return typowa d�ugo�� mi�dzy markerami
    float getStickLength( VectorChannelConstPtr channel1, VectorChannelConstPtr channel2, float epsilon = 0.001f );

private:
    //! kolekcja z markerami
    MarkerCollectionConstPtr markers;
    //! aktualny �rodek geometryczny
    osg::Vec3 currentPosition;
};
typedef core::shared_ptr<MarkersVisualizationScheme> MarkersVisualizationSchemePtr;
typedef core::shared_ptr<const MarkersVisualizationScheme> MarkersVisualizationSchemeConstPtr;

#endif
