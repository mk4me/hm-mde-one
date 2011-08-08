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
#include <plugins/chart/ChartDataSeries.h>
#include <plugins/c3d/C3DChannels.h>
#include "Chart.h"

class LineChartSerie : public ChartSerie
{
private:
    //! Tablica kolor�w.
    osg::ref_ptr<osg::Vec4Array> colors;
    //! Tablica wierzcho�k�w.
    osg::ref_ptr<osg::Vec3Array> vertices;
    //! Obs�uga czasu serii danych
    ScalarChannelConstPtr channel;
    //! Obs�uga czasu kana�u
    ScalarChannel::TimerPtr timer;
    //! Obs�uga statystyk kana�u
    ScalarChannelStatsPtr stats;
    //! Znormalizowany kana�
    ScalarChannelPtr normalizedChannel;

public:
    LineChartSerie();

public:
    //! \return Dane wykresu.
    inline const ScalarChannelConstPtr & getData() const
    { 
        return channel;
    }

    inline const ScalarChannel::TimerPtr & getTimer() const
    {
        return timer;
    }

    //! \param data Dane wykresu.
    void setData(const ScalarChannelConstPtr& channel);

// ChartSerie
public:
    //! Od�wie�a wykres. R�czne wywowy�awanie nie jest potrzebne, je�eli
    //! flaga autoRefresh jest zapalona.
    virtual void refresh();
    //! \return Zakres warto�ci dla osi X.
    virtual std::pair<float, float> getXRange() const;
    //! \return Zakres warto�ci dla osi Y.
    virtual std::pair<float, float> getYRange() const;
    //! \return Jednostka dla osi X.
    virtual const std::string& getXUnit() const;
    //! \return Jednostka dla osi Y.
    virtual const std::string& getYUnit() const;
    //!
    virtual float getValue() const;
    //!
    virtual float getTime() const;
};

typedef osg::ref_ptr<LineChartSerie> LineChartSeriePtr;


#endif