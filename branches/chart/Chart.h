/********************************************************************
created:  2010/08/02
filename: CoordinateSystem.h
author:	  Michal Szafarski

purpose:  Klasa ta sluzy do tworzenia wykresu
*********************************************************************/
#ifndef __HEADER_GUARD__CHART_H__
#define __HEADER_GUARD__CHART_H__

#include <osg/Geode>
#include <osg/Geometry>



class Chart : public osg::Group{
private:
	//! kolor wykresu
	osg::Vec4Array* colors ;
	//! wierzcholki wykresu

public:
	//! Funkcja rysujaca wykres na podstawie podanych wierzcholkow
	void drawChart(osg::Vec3Array* chartVertices);
	//! Funkcja ustawiajaca kolor wykresu
	void setColor(float,float,float,float);
};


#endif