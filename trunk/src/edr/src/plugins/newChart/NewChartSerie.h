/********************************************************************
	created:	2011/11/12
	created:	12:11:2011   18:08
	filename: 	ChartSerie.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__NEWCHARTSERIE_H__
#define HEADER_GUARD_NEW_CHART__NEWCHARTSERIE_H__

#include <core/IVisualizer.h>
#include <core/IObjectSource.h>
#include <plugins/c3d/C3DCollections.h>
#include <plugins/c3d/EventSerieBase.h>
#include <plugins/newChart/INewChartSerie.h>
#include "NewChartCurve.h"
#include "NewChartEvents.h"
#include "Scales.h"

class NewChartCurve;

//! Podstawowa seria danych wizualizatora 2D
class NewChartSerie : public INewChartSerie
{
    friend class NewChartVisualizer;
public:
    //! Konstruktor
    //! \param visualizer wizualzator, ktory tworzy serie danych
    NewChartSerie(NewChartVisualizer * visualizer);
    virtual ~NewChartSerie();

public:
    //! \return czas ustawiony przez timeline
    double getTime() const;
    //! \return aktualna wartosc (dla aktualnego czasu)
    double getCurrentValue() const;
    //! ustawia czas dla serii
    //! \param time czas do ustawienia
    virtual void setTime(double time);
    //! \return czas trwania serii
    virtual double getLength() const;
    //! ustawia obiekt ze zdarzeniami powiazanymi z proba pomiarowa
    //! \param val kolecja zdarzen
    virtual void setEvents(EventsCollectionConstPtr val);
    //! pobierz krzywa qwt
    const QwtPlotCurve* getCurve() const; 
    //! ustaw widzialnosc krzywej
    //! \param visible widoczna / niewidoczna
    void setVisible(bool visible);
    //! \return czy krzywa jest widoczna
    bool isVisible() const;
    //! \return obiekt ulatwiajacy zarzadzanie eventami
    EventsHelperPtr getEventsHelper() const { return eventsHelper; }
    //! ustawienie koloru krzywej
    //! \param r skladowa czerwona (0 - 255)
    //! \param g skladowa zielona (0 - 255)
    //! \param b skladowa niebieska (0 - 255)
    //! \param a skladowa alpha (0 - 255)
    void setColor(int r, int g, int b, int a = 255);
    //! ustawienie koloru krzywej
    //! \param color kolor do ustawienia
    virtual void setColor(const QColor& color);
    //! \return kolor krzywej
    virtual QColor getColor() const;
    //! ustawia szerokosc krzywej
    //! \param width nowa szerokosc w pikselach
    void setWidth(int width);
    //! ustawia nazwe krzywej
    //! \param name nowa nazwa krzywej
    virtual void setName(const std::string & name);
    //! \return nazwa krzywej
    virtual const std::string & getName() const;
    //! dostarcza ObjectWrapper z danymi 
    //! \param data ObjectWrapper przechowujacy interfejs ScalarChannelReaderInterface
    virtual void setData(const core::ObjectWrapperConstPtr & data);
    //! \return ObjectWrapper przechowujacy interfejs ScalarChannelReaderInterface
    virtual const core::ObjectWrapperConstPtr & getData() const;
    //! \return ekstrema krzywej
    Scales getScales() const; 
    //! \return czy krzywa jest aktywna
    bool isActive() const { return active; }
    //! ustawia aktywnosc krzywej
    //! \param val aktywna / nieaktywna
    void setActive(bool val);
    //! \return statystyki dla ustawionego ScalarChannelReaderInterface
    ScalarChannelStatsConstPtr getStats() const { return stats; } //return pointHelper->getStats(); }
    //! \return ustawiony ScalarChannelReaderInterface
    ScalarChannelReaderInterfaceConstPtr getReader() const { return reader; }
    //! odlacza krzywa od wykresu
    void removeItemsFromPlot();
    //! ustawia kolejnosc wyswietlania
    //! \param z 
    //! \param replot 
    void setZ(double z, bool replot = false);
    //! \return skladowa z odpowiedzialna z kolejnosc wyswietlania
    double z() const;
    //! \return skala w poziomie
    double getXScale() const;
    //! ustawia skale w poziomie dla krzywej
    //! \param val nowa wartosc skali
    void setXScale(double val);
    //! \return skala w pionie
    double getYScale() const;
    //! ustawia skale w pionie dla krzywej
    //! \param val nowa wartosc skali
    void setYScale(double val);
    //! \return przesuniecie w poziomie
    double getXOffset() const;
    //! ustawia przesuniecie w poziomie
    //! \param val nowa wartosc przesuniecia
    void setXOffset(double val);
    //! \return przesuniecie w pionie
    double getYOffset() const;
    //! ustawia przesuniecie w pionie
    //! \param val nowa wartosc przesuniecia
    void setYOffset(double val);
    //! \return przesuniecie krzywej
    QPointF getOffset() const;
    //! ustawia przesuniecie 
    //! \param offset nowa wartosc przesuniecia
    void setOffset(const QPointF& offset);

private:
    //! Ustawia kolory dla krzywych znormalizowanych wzgledem eventow
    //! \param range dostarczane segmenty
    //! \param color kolor dla ustawianych segmentow (zwykle ten sam co bazowa krzywa)
    void setColorsForEvents(EventsHelper::SegmentsRange range, const QColor& color);

private:
    //! wizualizator, ktory stworzyl serie
    NewChartVisualizer* visualizer;
    //! nazwa serii
    std::string name;
    //! OW z danymi
    core::ObjectWrapperConstPtr data;
    //! wizualizowana krzywa
    NewChartCurve* curve;
    //! obiekt ze statystykami
    ScalarChannelStatsPtr stats;
    //! aksesor do danych w DataChannel
    core::shared_ptr<ScalarContiniousTimeAccessor> accessor;
    //! obiekt z danymi
    ScalarChannelReaderInterfaceConstPtr reader;
    //! obiekt zajmujacy sie logika eventow
    EventsHelperPtr eventsHelper;
    //! czy seria jest aktywna
    bool active;
    //! aktualny czas, ustawiony przez timeline
    double time;
    //! skladowa okresla kolejnosc rysowania
    double _z;
    //! poczatkowa wartosc skladowej zd
    double _zBase;    
};

#endif
