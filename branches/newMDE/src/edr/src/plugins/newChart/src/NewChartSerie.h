/********************************************************************
	created:	2011/11/12
	created:	12:11:2011   18:08
	filename: 	ChartSerie.h
	author:		Wojciech Kniec

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__NEWCHARTSERIE_H__
#define HEADER_GUARD_NEW_CHART__NEWCHARTSERIE_H__

#include <corelib/IVisualizer.h>
#include <plugins/c3d/C3DCollections.h>
#include <plugins/c3d/EventSerieBase.h>
#include <plugins/newChart/INewChartSerie.h>
#include "NewChartCurve.h"
#include "NewChartEvents.h"
#include "Scales.h"

class NewChartCurve;
class NewChartVisualizer;

//! Podstawowa seria danych wizualizatora 2D
class NewChartSerie : public INewChartSerie
{
    friend class NewChartVisualizer;
public:
    //! Konstruktor
    //! \param visualizer wizualzator, który tworzy serie danych
    NewChartSerie(NewChartVisualizer * visualizer);
    virtual ~NewChartSerie();

public:
    //! \return czas ustawiony przez timeline
    double getTime() const;
    //! \return aktualna wartość (dla aktualnego czasu)
    double getCurrentValue() const;
    //! ustawia czas dla serii
    //! \param time czas do ustawienia
    virtual void setTime(double time);
    //! \return czas trwania serii
    virtual double getLength() const;
    //! ustawia obiekt ze zdarzeniami powiazanymi z próbą pomiarową
    //! \param val kolecja zdarzeń
    virtual void setEvents(EventsCollectionConstPtr val);
    //! pobierz krzywa qwt
    const QwtPlotCurve* getCurve() const;
    //! ustaw widzialność krzywej
    //! \param visible widoczna / niewidoczna
    void setVisible(bool visible);
    //! \return czy krzywa jest widoczna
    bool isVisible() const;
    //! \return obiekt ułatwiający zarządzanie eventami
    EventsHelperPtr getEventsHelper() const { return eventsHelper; }
    //! ustawienie koloru krzywej
    //! \param r składowa czerwona (0 - 255)
    //! \param g składowa zielona (0 - 255)
    //! \param b składowa niebieska (0 - 255)
    //! \param a składowa alpha (0 - 255)
    void setColor(int r, int g, int b, int a = 255);
    //! ustawienie koloru krzywej
    //! \param color kolor do ustawienia
    virtual void setColor(const QColor& color);
    //! \return kolor krzywej
    virtual QColor getColor() const;
    //! ustawia szerokość krzywej
    //! \param width nowa szerokość w pikselach
    void setWidth(int width);
    //! ustawia nazwę krzywej
    //! \param name nowa nazwa krzywej
    virtual void setName(const std::string & name);
    //! \return nazwa krzywej
    virtual const std::string getName() const;
    //! dostarcza ObjectWrapper z danymi
    //! \param data ObjectWrapper przechowujący interfejs ScalarChannelReaderInterface
    virtual void setData(const utils::TypeInfo & requestedData, const core::ObjectWrapperConstPtr & data);
    //! \return ObjectWrapper przechowujący interfejs ScalarChannelReaderInterface
    virtual const core::ObjectWrapperConstPtr & getData() const;

	virtual const utils::TypeInfo & getRequestedDataType() const;

	virtual void update();

    //! \return ekstrema krzywej
    Scales getScales() const;
    //! \return czy krzywa jest aktywna
    bool isActive() const { return active; }
    //! ustawia aktywność krzywej
    //! \param val aktywna / nieaktywna
    void setActive(bool val);
    //! \return statystyki dla ustawionego ScalarChannelReaderInterface
    ScalarChannelStatsConstPtr getStats() const { return stats; } //return pointHelper->getStats(); }
    //! \return ustawiony ScalarChannelReaderInterface
    ScalarChannelReaderInterfaceConstPtr getReader() const { return reader; }
    //! odlacza krzywa od wykresu
    void removeItemsFromPlot();
    //! ustawia kolejność wyświetlania
    //! \param z
    //! \param replot
    void setZ(double z, bool replot = false);
    //! \return składowa z odpowiedzialna z kolejność wyświetlania
    double z() const;
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

private:
    //! Ustawia kolory dla krzywych znormalizowanych względem eventów
    //! \param range dostarczane segmenty
    //! \param color kolor dla ustawianych segmentów (zwykle ten sam co bazowa krzywa)
    void setColorsForEvents(EventsHelper::SegmentsRange range, const QColor& color);

private:
    //! wizualizator, który stworzył serie
    NewChartVisualizer* visualizer;
    //! nazwa serii
    std::string name;
    //! OW z danymi
    core::ObjectWrapperConstPtr data;

	utils::TypeInfo requestedType;
    //! wizualizowana krzywa
    NewChartCurve* curve;
    //! obiekt ze statystykami
    ScalarChannelStatsPtr stats;
    //! aksesor do danych w DataChannel
    utils::shared_ptr<ScalarContiniousTimeAccessor> accessor;
    //! obiekt z danymi
    ScalarChannelReaderInterfaceConstPtr reader;
    //! obiekt zajmujący się logika eventów
    EventsHelperPtr eventsHelper;
    //! czy seria jest aktywna
    bool active;
    //! aktualny czas, ustawiony przez timeline
    double time;
    //! składowa okresla kolejność rysowania
    double _z;
    //! początkowa wartość składowej zd
    double _zBase;
};

#endif
