/********************************************************************
created:  2010/09/21
filename: LineChartSerie.h
author:	  Michal Szafarski

purpose:  Klasa ta sluzy do tworzenia wykresu liniowego
*********************************************************************/
#ifndef __HEADER_GUARD__LINECHART_H__
#define __HEADER_GUARD__LINECHART_H__

#include <osg/Geode>
#include <osg/Geometry>
#include <plugins/chart/ChartDataSeries.h>
#include <core/C3DChannels.h>
#include <core/ChartSerie.h>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

class LineChartSerie : public ChartSerie
{
private:
    //! Tablica kolorów.
    osg::ref_ptr<osg::Vec4Array> colors;
    //! Tablica wierzcho³ków.
    osg::ref_ptr<osg::Vec3Array> vertices;
    //! Faktyczny wykres.
    osg::ref_ptr<osg::Geometry> lineChart;
    //! Dane wykresu.
    ScalarChannelPtr data;

public:
    LineChartSerie();

public:
    //! \return Dane wykresu.
    inline ScalarChannelConstPtr getData() const
    { 
        return data;
    }
    //! \param data Dane wykresu.
    void setData(const ScalarChannelPtr& data);

// core::ChartSerie
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
};

typedef osg::ref_ptr<LineChartSerie> LineChartSeriePtr;

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////


#endif