/********************************************************************
	created:	2012/04/30
	created:	30:4:2012   13:19
	filename: 	INewChartVisualizer.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__INEWCHARTVISUALIZER_H__
#define HEADER_GUARD_NEW_CHART__INEWCHARTVISUALIZER_H__

#include <corelib/IVisualizer.h>

//! niektóre elementy wizualizatora wykresów są edytowane z zewnątrz, 
//! dlatego należy interfejsować serie danych, aby dać dostęp do obiektu bez konieczności linkowania się z pluginem
//! taką funkcjonalność w obrębie wizualizatora zapewnia właśnie ten interfejs
class INewChartVisualizer : public plugin::IVisualizer
{
public:
	virtual ~INewChartVisualizer() {}

public:
    //! Ustawia tytuł całego wykresu
    //! \param title nowy tytuł
    virtual void setTitle( const QString& title ) = 0;
    //! \return tytuł wykresu 
    virtual QString getTitle() const = 0;
};
typedef utils::shared_ptr<INewChartVisualizer> INewChartVisualizerPtr;
typedef utils::shared_ptr<const INewChartVisualizer> INewChartVisualizerConstPtr;


#endif
