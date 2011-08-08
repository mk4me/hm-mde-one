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
    //! Tablica kolorów.
    osg::ref_ptr<osg::Vec4Array> colors;
    //! Tablica wierzcho³ków.
    osg::ref_ptr<osg::Vec3Array> vertices;
    //! Obs³uga czasu serii danych
    ScalarChannelConstPtr channel;
    //! Obs³uga czasu kana³u
    ScalarChannel::TimerPtr timer;
    //! Obs³uga statystyk kana³u
    ScalarChannelStatsPtr stats;
    //! Znormalizowany kana³
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
    //! Odœwie¿a wykres. Rêczne wywowy³awanie nie jest potrzebne, je¿eli
    //! flaga autoRefresh jest zapalona.
    virtual void refresh();
    //! \return Zakres wartoœci dla osi X.
    virtual std::pair<float, float> getXRange() const;
    //! \return Zakres wartoœci dla osi Y.
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