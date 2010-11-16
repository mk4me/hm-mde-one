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
#include "ChartData.h"
#include <boost/lexical_cast.hpp>
#include <iomanip>
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
	osg::Group* createBorder();
	//! Tworzy siatke dla wykresu
	osg::Group* createGrid(osg::Vec3 startPoint,osg::Vec3 endPoint);
	//! tworzy linie do siatki
	osg::Node* createLine(osg::Vec3 startPoint,osg::Vec3 endPoint,osg::Vec4 lineColor);
	//! odleglosc ramki od wykresu
	int borderSize;
	//! gestosc siatki
	int gridDensity;
	//! ramka
	osg::Group* border;
	//! siatka
	osg::Group* grid;
	//! os X
	osg::Group* xAxis;
	//! os Y
	osg::Group* yAxis;
	//! metoda inicjalizujaca
	void init();
	//! dane
	ChartData* data;
	//! kolor siatki
	osg::Vec4 gridColor; 
	//! rozmiar czcionki
	int fontSize;
	//! ilosc przedzialek na osi X
	int xNumReps;
	//! ilosc przedzialek na osi Y
	int yNumReps;
std::string formatNumber( float number );
public:
	//! Konstruktor bez sprecyzowanego koloru ukladu
	ChartDecoration(osg::Vec3,float,float,ChartData* data);
	//! Funkcja zwracajaca skale osi X
	float getScaleX();
	//! Funkcja zwracajaca skale osi Y
	float getScaleY();
	//! Funkcja ustawiajaca skale osi X
	osg::Group* setScaleX(float scale);
	//! Funkcja ustawiajaca skale osi Y
	osg::Group* setScaleY(float scale);
	//! Funkcja zwracajaca poczatkowa wartosci skali osi X
	float getSScaleX();
	//! Funkcja zwracajaca poczatkowa wartosci skali osi Y
	float getSScaleY();
	//! Funkcja zwracajaca punkt poczatkowy wykresu
	osg::Vec3 getStartPoint();
	//! Funkcja ustawiajaca punkt poczatkowy wykresu
	void setStartPoint(osg::Vec3 startPoint);
	//! Funkcja zwracajaca punkt koncowy wykresu
	osg::Vec3 getEndPoint();
	//! Funkcja zwracajaca dlugosc osi X
	float getXEnd();
	//! Funkcja zwracajaca dlugosc osi Y
	float getYEnd();
	//! Funkcja ustawiajaca dlugosc osi X
	void setXEnd(float xEnd);
	//! Funkcja ustawiajaca dlugosc osi Y
	void setYEnd(float yEnd);
	//! Funkcja zwracajaca odstep ramki od wykresu
	int getBorderSize();
	//! Funkcja ustawiajaca odstep ramki od wykres
	void setBorderSize(int borderSize);
	//! Funkcja zwracajaca wielkosc czcionki
	int getFontSize();
	//! Funkcja ustawiajaca wielkosc czcionki 
	void setFontSize(int fontSize);
	//! Funkcja pobierajaca gestosc siatki. Ile pixeli szerokosci ma jedna kratka
	int getGridDensity();
	//! Funkcja ustawiajaca gestosc siatki. Ile pixeli szerokosci ma jedna kratka
	void setGridDensity(int gridDensity);
	//! Funkcja pobierajaca kolor siatki w rgba
	osg::Vec4 getGridColor();
	//! Funkcja ustawiajaca kolor siatki w rgba
	void setGridColor(osg::Vec4 gridColor);
	//! Funkcja pobierajaca kolor wykresu w rgba
	osg::Vec4 getColor();
	//! Funkcja ustawiajaca kolor wykresu w rgba
	void setColor(osg::Vec4 color);
	//! Funkcja odswieza wszystkie elementy dekoracji
	void repaint();
	//! Funkcja pobierajaca ilosc przedzialek na osi X
	int getXNumReps();
	//! Funkcja ustawiajaca ilosc przedzialek na osi X
	void setXNumReps(int xNumReps);
	//! Funkcja pobierajaca ilosc przedzialek na osi Y
	int getYNumReps();
	//! Funkcja ustawiajaca ilosc przedzialek na osi Y
	void setYNumReps(int xNumReps);
};
#endif