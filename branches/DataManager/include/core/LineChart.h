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
class ChartData;
class ChartPointer;
class LineChart : public ChartDataSeries{
private:
	osg::Vec3Array* chartVertices;
	ChartPointer* pointer;
	osg::Geode* chart;
	ChartData* data;
	bool showLabel;
	int x,y,width,height;
public:
	//konstruktor wykresu
	LineChart(ChartData* data,int x,int y,int width, int height,osg::Vec4 color);
~LineChart();
	//! Funkcja dodajaca kolejny wierzcholek
	void addCoord(osg::Vec3);
	ChartPointer* getPointer();
	void repaint(ChartData* data,int x,int y,int width,int height);
	void setShowLabel(bool showLabel);
};


#endif