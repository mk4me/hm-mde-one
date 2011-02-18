/********************************************************************
created:  2010/09/21
filename: LineChart.h
author:	  Michal Szafarski

purpose:  Klasa ta sluzy do tworzenia wykresu liniowego
*********************************************************************/
#ifndef __HEADER_GUARD__LINECHART_H__
#define __HEADER_GUARD__LINECHART_H__

#include <osg/Geode>
#include <osg/Geometry>
#include <plugins/chart/ChartDataSeries.h>
#include <core/C3DChannels.h>

class deprecated_ChartData;
class ChartPointer;
class deprecated_LineChart : public ChartDataSeries{
private:
	osg::Vec3Array* chartVertices;
	ChartPointer* pointer;
	osg::Geode* chart;
	deprecated_ChartData* data;
	bool showLabel;
	int x,y,width,height;
public:
	//konstruktor wykresu
	deprecated_LineChart(deprecated_ChartData* data,int x,int y,int width, int height,osg::Vec4 color);
~deprecated_LineChart();
	//! Funkcja dodajaca kolejny wierzcholek
	void addCoord(osg::Vec3);
	ChartPointer* getPointer();
	void repaint(deprecated_ChartData* data,int x,int y,int width,int height);
	void setShowLabel(bool showLabel);
};

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

class LineChart : public ChartDataSeries
{
private:
    osg::ref_ptr<osg::Vec3Array> vertices;
    osg::ref_ptr<osg::Geode> lineChart;
    bool autoRefresh;
    float verticesPerUnit;
    float x;
    float y;
    float w;
    float h;
    float z;
    ScalarChannelPtr data;

public:
    LineChart();

public:
    //! \return
    bool isAutoRefresh() const
    { 
        return autoRefresh;
    }
    //! \param autoRefresh
    void setAutoRefresh(bool autoRefresh) 
    { 
        this->autoRefresh = autoRefresh; 
    }

    //! \return Maksymalna liczba wierzcho³ków per jednostka miary. Domyœlnie 1.
    float getVerticesPerUnit() const
    { 
        return verticesPerUnit;
    }
    //! \return Maksymalna liczba wierzcho³ków per jednostka miary. Domyœlnie 1.
    void setVerticesPerUnit(float verticesPerUnit);

    //! \param data
    void setData(ScalarChannelPtr data);
    //! \param x
    //! \param y
    //! \param w
    //! \param h
    void setLocation(float x, float y, float w, float h);
    //! \param location
    //! \param size
    void setLocation(osg::Vec2 location, osg::Vec2 size);
    //! 
    osg::Vec2 getPosition() const
    {
        return osg::Vec2(x, y);
    }
    //! 
    osg::Vec2 getSize() const
    {
        return osg::Vec2(w, h);
    }

    //! Odœwie¿a wykres. Rêczne wywowy³awanie nie jest potrzebne, je¿eli
    //! flaga autoRefresh jest zapalona.
    void refresh();

private:
    //! Odœwie¿a wykres, o ile flaga autoRefresh jest zapalona.
    void tryRefresh();
};

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////


#endif