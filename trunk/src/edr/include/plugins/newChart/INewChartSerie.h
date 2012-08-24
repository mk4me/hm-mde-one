/********************************************************************
	created:	2012/04/30
	created:	30:4:2012   13:19
	filename: 	INewChartSerie.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__INEWCHARTSERIE_H__
#define HEADER_GUARD_NEW_CHART__INEWCHARTSERIE_H__

#include <plugins/c3d/EventSerieBase.h>

//! Niektóre elementy serii danych wizualizatora wykresów s¹ edytowane z zewn¹trz, 
//! dlatego nale¿y interfejsowaæ serie danych, aby daæ dostêp do obiektu bez koniecznoœci linkowania siê z pluginem
//! tak¹ funkcjonalnoœæ w obrêbie serii zapewnia w³aœnie ten interfejs
class INewChartSerie : public EventSerieBase
{
public:
	//! destruktor
	virtual ~INewChartSerie() {}

public:
    //! pobiera kolor serii
    virtual QColor getColor() const = 0;
    //! ustawia kolor serii
    //! \param color kolor w formacie Qt
    virtual void setColor(const QColor& color) = 0;
};
typedef core::shared_ptr<INewChartSerie> INewChartSeriePtr;
typedef core::shared_ptr<const INewChartSerie> INewChartSerieConstPtr;


#endif
