/********************************************************************
created:  2010/11/17
filename: Chart.h
author:	  Michal Szafarski

purpose:  Klasa ta zarzadza calym wykresem
*********************************************************************/
#ifndef __HEADER_GUARD__Chart_H__
#define __HEADER_GUARD__Chart_H__

#include <osg/Geode>
#include <osg/Geometry>
#include "ChartDecoration.h"
#include "LineChart.h"
#include "ChartData.h"
class ChartWidget;

class Chart : public osg::Group{
private:
	int chartCount;
	int chartDecorationCount;
	std::string c3dFile;
	ChartWidget* chartWidget;
	int index;
	ChartData** data;
	ChartDecoration** chartDecoration;
	LineChart** chart;
	void setChartCount(int chartCount);
	void setChartDecorationCount(int chartDecorationCount);
public:
	Chart(ChartWidget* chartWidget,std::string c3dFile);
	~Chart();
	LineChart* getChart(int index);
	ChartData* getData(int index);
	ChartDecoration* getDecoration(int index);
	//! dodaje uklad wspolezdnych oraz wykres o zadanym indexie
	void addChartDecoration(int index);
	//! pobiera ilosc wykresow w scenie
	int getChartCount();
	//! pobiera ilosc kompletnych ukladow na scenie
	int getChartDecorationCount();
	//! Dodaje wykres do wybranego ukladu TODO poprawic skalowanie i wskazniki przy kilku wykresach w ukladzie
	void addChart(int chartDecorationIndex,int index);
	//! Funkcja pobierajaca ilosc przedzialek na osi X
	int getXNumReps(int chartDecorationIndex);
	//! Funkcja ustawiajaca ilosc przedzialek na osi X
	void setXNumReps(int chartDecorationIndex,int xNumReps);
	//! Funkcja pobierajaca ilosc przedzialek na osi Y
	int getYNumReps(int chartDecorationIndex);
	//! Funkcja ustawiajaca ilosc przedzialek na osi Y
	void setYNumReps(int chartDecorationIndex,int xNumReps);
	//! Funkcja zwracajaca dlugosc osi X
	float getWidth(int chartDecorationIndex);
	//! Funkcja zwracajaca dlugosc osi Y
	float getHeight(int chartDecorationIndex);
	//! Funkcja ustawiajaca dlugosc osi X
	void setWidth(int chartDecorationIndex,float width);
	//! Funkcja ustawiajaca dlugosc osi Y
	void setHeight(int chartDecorationIndex,float height);
	//! Funkcja zwracajaca odstep ramki od wykresu
	int getBorderSize(int chartDecorationIndex);
	//! Funkcja ustawiajaca odstep ramki od wykres
	void setBorderSize(int chartDecorationIndex,int borderSize);
	//! Funkcja zwracajaca wielkosc czcionki
	int getFontSize(int chartDecorationIndex);
	//! Funkcja ustawiajaca wielkosc czcionki 
	void setFontSize(int chartDecorationIndex,int fontSize);
	//! Funkcja pobierajaca gestosc siatki. Ile pixeli szerokosci ma jedna kratka
	int getGridDensity(int chartDecorationIndex);
	//! Funkcja ustawiajaca gestosc siatki. Ile pixeli szerokosci ma jedna kratka
	void setGridDensity(int chartDecorationIndex,int gridDensity);
	//! Funkcja pobierajaca kolor siatki w rgba
	osg::Vec4 getGridColor(int chartDecorationIndex);
	//! Funkcja ustawiajaca kolor siatki w rgba
	void setGridColor(int chartDecorationIndex,osg::Vec4 gridColor);
	//! Funkcja pobierajaca kolor wykresu w rgba
	osg::Vec4 getColor(int chartDecorationIndex);
	//! Funkcja ustawiajaca kolor wykresu w rgba
	void setColor(int chartDecorationIndex,osg::Vec4 color);
	//! przesuwa wykres o wybranym indexie
	void setLocation(int chartDecorationIndex,int x,int y,int width,int height);
	//! pobiera lokacje wykresu o wpisanym indexie
	osg::Vec4 getLocation(int chartDecorationIndex);
	//! pobiera ilosc klatek
	int getFrameNumber(int chartIndex);
	//! pobiera ile klatek wyswietla sie w ciagu sekundy
	int getFPS(int chartIndex);
	//! odswieza pozycje wskaznika
	void updatePointer(double targetTime);
};


#endif