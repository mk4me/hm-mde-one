///********************************************************************
//created:  2010/09/21
//filename: LineChartSerie.h
//author:	  Michal Szafarski
//
//purpose:  Klasa ta sluzy do tworzenia wykresu liniowego
//*********************************************************************/
#ifndef __HEADER_GUARD__LINECHART_H__
#define __HEADER_GUARD__LINECHART_H__

#include <osg/Geode>
#include <osg/Geometry>
#include "ChartSerie.h"
#include <plugins/c3d/C3DChannels.h>
#include "Chart.h"

class LineChartSerie : public ChartSerie
{
private:
    //! Tablica kolorów.
    osg::ref_ptr<osg::Vec4Array> colors;
    //! Tablica wierzchołków.
    osg::ref_ptr<osg::Vec3Array> vertices;
    //! Obsługa czasu serii danych
    ScalarChannelReaderInterfaceConstPtr channel;
    //! Obsługa czasu kanału
    GeneralTimer::TimerPtr timer;
    //! Obsługa statystyk kanału
    ScalarChannelStatsPtr stats;
    //! Znormalizowany kanał
    boost::shared_ptr<ScalarModifier> normalizedChannel;

    boost::shared_ptr<ScalarContiniousTimeAccessor> accessor;
    boost::shared_ptr<ScalarCurentValueExtractor> currentValue;
    //!
    bool active;

public:
    LineChartSerie();

public:
    //! \return Dane wykresu.
    inline const ScalarChannelReaderInterfaceConstPtr & getData() const
    { 
        return channel;
    }

    inline const GeneralTimer::TimerPtr & getTimer() const
    {
        return timer;
    }

    //! \param data Dane wykresu.
    void setData(const ScalarChannelReaderInterfaceConstPtr& channel);

// ChartSerie
public:
    //! Odświeża wykres. Ręczne wywowyławanie nie jest potrzebne, jeżeli
    //! flaga autoRefresh jest zapalona.
    virtual void refresh();
    //! \return Zakres wartości dla osi X.
    virtual std::pair<float, float> getXRange() const;
    //! \return Zakres wartości dla osi Y.
    virtual std::pair<float, float> getYRange() const;
    //! \return Jednostka dla osi X.
    virtual const std::string& getXUnit() const;
    //! \return Jednostka dla osi Y.
    virtual const std::string& getYUnit() const;
    //!
    virtual float getValue() const;
    //!
    virtual float getTime() const;
    //!
    virtual bool isActive() const;
    //!
    virtual void setActive(bool active);
};

typedef osg::ref_ptr<LineChartSerie> LineChartSeriePtr;


#endif
