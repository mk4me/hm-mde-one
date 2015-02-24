/********************************************************************
	created:	2011/11/12
	created:	12:11:2011   18:08
	filename: 	ChartSerie.h
	author:		Wojciech Kniec

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__INEWCHARTSERIEPRIVATE_H__
#define HEADER_GUARD_NEW_CHART__INEWCHARTSERIEPRIVATE_H__

#include <corelib/IVisualizer.h>
#include <plugins/c3d/EventSerieBase.h>
#include <plugins/newChart/INewChartSerie.h>
#include "NewChartEvents.h"
#include "Scales.h"
#include <corelib/IVisualizer.h>

class NewChartCurve;
class NewChartVisualizer;

//! Podstawowa seria danych wizualizatora 2D
class INewChartSeriePrivate : public INewChartSerie
{
public:
    //! pobierz krzywą qwt
    virtual const QwtPlotCurve* getCurve() const = 0;
   //! pobierz krzywą qwt
    virtual QwtPlotCurve* getCurve() = 0;
   //! ustaw widzialność krzywej
   //! \param visible widoczna / niewidoczna
    virtual void setVisible(bool visible) = 0;
   //! \return czy krzywa jest widoczna
    virtual bool isVisible() const = 0;
   //! \return obiekt ułatwiający zarządzanie eventami
    virtual EventsHelperPtr getEventsHelper() const { return EventsHelperPtr(); }
    virtual bool isActive() const = 0;
	//! ustawia aktywność krzywej
	//! \param val aktywna / nieaktywna
    virtual void setActive(bool val) = 0;
    virtual Scales getScales() const = 0;
    //! ustawia szerokość krzywej
    //! \param width nowa szerokość w pikselach
    virtual void setWidth(int width) = 0;
    //! odlacza krzywa od wykresu
    virtual void removeItemsFromPlot() = 0;
    //! ustawia kolejność wyświetlania
    //! \param z
    //! \param replot
    virtual void setZ(double z, bool replot = false)= 0;
    //! \return składowa z odpowiedzialna z kolejność wyświetlania
    virtual double z() const= 0;

    const plugin::IVisualizer::ISerie* asISerie() const { return dynamic_cast<const plugin::IVisualizer::ISerie*>(this); }
    plugin::IVisualizer::ISerie* asISerie()  { return dynamic_cast<plugin::IVisualizer::ISerie*>(this); }

public:/*
    //! \return czas ustawiony przez timeline
    double getTime() const;
    //! \return aktualna wartość (dla aktualnego czasu)
    double getCurrentValue() const;
    //! ustawia nazwę krzywej
    //! \param name nowa nazwa krzywej
    virtual void setName(const std::string & name);
    //! \return nazwa krzywej
    virtual const std::string getName() const;


    //! \return skala w poziomie
    double getXScale() const;
    //! ustawia skale w poziomie dla krzywej
    //! \param val nowa wartość skali
    void setXScale(double val);
    //! \return skala w pionie
    double getYScale() const;
    //! ustawia skale w pionie dla krzywej
    //! \param val nowa wartość skali
    void setYScale(double val);
    //! \return przesunięcie w poziomie
    double getXOffset() const;
    //! ustawia przesunięcie w poziomie
    //! \param val nowa wartość przesunięcia
    void setXOffset(double val);
    //! \return przesunięcie w pionie
    double getYOffset() const;
    //! ustawia przesunięcie w pionie
    //! \param val nowa wartość przesunięcia
    void setYOffset(double val);
    //! \return przesunięcie krzywej
    QPointF getOffset() const;
    //! ustawia przesunięcie
    //! \param offset nowa wartość przesunięcia
    void setOffset(const QPointF& offset);
*/
};

#endif
