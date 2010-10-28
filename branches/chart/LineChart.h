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
#include "ChartData.h"
#include "Chart.h"
#include "ChartDecoration.h"
#include "Pointer.h"

class LineChart : public Chart{
private:
	osg::Vec3Array* chartVertices;
public:
	//konstruktor wykresu
	LineChart(ChartData* data,ChartDecoration* chartDecoration);

	//! Funkcja dodajaca kolejny wierzcholek
	void addCoord(osg::Vec3);

};


#endif