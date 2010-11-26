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

	int x,y,width,height;
public:
	//konstruktor wykresu
	LineChart(ChartData* data,int x,int y,int width, int height);

	//! Funkcja dodajaca kolejny wierzcholek
	void addCoord(osg::Vec3);
	ChartPointer* getPointer();
	void repaint(int x,int y,int width,int height);
};


#endif