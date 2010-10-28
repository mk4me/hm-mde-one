/********************************************************************
	created:  2010/09/23
	filename: ChartDecoration.h
	author:	  Michal Szafarski
	
	purpose:  Klasa ta sluzy do tworzenia ukladu wspolzednych
*********************************************************************/
#ifndef __HEADER_GUARD__CHARTDECORATION_H__
#define __HEADER_GUARD__CHARTDECORATION_H__

#include <iostream>
#include <sstream>
#include <string>
#include <osg/Geode>
#include <osgText/Text>
#include <osg/AutoTransform>
#include <osg/Geometry>
#include "Pointer.h"
class ChartDecoration : public osg::Group{
private:
	//! Funkcja zamieniajaca wartosci float na string
	std::string float2str(float);
	//! Funkcja sluzaca do tworzenia elementow tekstowych
	osg::Node* createLabel(const osg::Vec3& pos, float size, const std::string& label);
	//! Funkcja rysujaca os ukladu wspolzednych
	osg::Node* createAxis(const osg::Vec3& s, const osg::Vec3& e, int numReps, float scale);
	//! Skala osi X
	float scaleX;
	//! Skala osi Y
	float scaleY;
	//! Poczatkowa wartosc skali osi X
	float sScaleX;
	//! Poczatkowa wartosc skali osi Y
	float sScaleY;
	//! Miejsce zerowe wykresu
	osg::Vec3 startPoint;
	//! Dlugosc osi X
	float xEnd;
	//! Dlugosc osi Y
	float yEnd;
	//! Kolor bazowy
	osg::Vec4 color;
	//! Tworzy ramke wokol wykresu
	void createBorder(osg::Vec3 startPoint,float xEnd,float yEnd);
	//! Tworzy siatke dla wykresu
	void createGrid(osg::Vec3 startPoint,osg::Vec3 endPoint);
	//! tworzy linie do siatki
	osg::Node* createLine(osg::Vec3 startPoint,osg::Vec3 endPoint,osg::Vec4 lineColor);
public:
	//! Konstruktor bez sprecyzowanego koloru ukladu
	ChartDecoration(osg::Vec3,float,float);
	//! Konstruktor z sprecyzowanym kolorem ukladu
	ChartDecoration(osg::Vec3,float,float,osg::Vec4);
	//! Funkcja zwracajaca skale osi X
	float getScaleX();
	//! Funkcja zwracajaca skale osi Y
	float getScaleY();
	//! Funkcja ustawiajaca skale osi X
	void setScaleX(float scale);
	//! Funkcja ustawiajaca skale osi Y
	void setScaleY(float scale);
	//! Funkcja zwracajaca poczatkowa wartosci skali osi X
	float getSScaleX();
	//! Funkcja zwracajaca poczatkowa wartosci skali osi Y
	float getSScaleY();
	//! Funkcja zwracajaca punkt poczatkowy wykresu
	osg::Vec3 getStartPoint();
	//! Funkcja zwracajaca punkt koncowy wykresu
	osg::Vec3 getEndPoint();
	//! Funkcja zwracajaca dlugosc osi X
	float getXEnd();
	//! Funkcja zwracajaca dlugosc osi Y
	float getYEnd();
};
#endif