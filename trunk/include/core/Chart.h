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

class LineChart;
class ChartData;

class Chart : public osg::Group{
private:

	int x,y,width,height;
	std::vector<LineChart*> dataSeries;
	std::vector<ChartData*> data;
	void init();
	//! odleglosc ramki od wykresu
	int borderSize,fontSize;
	osg::Node* createLine(int x,int y,int x1,int y1,int z,osg::Vec4 lineColor);
	osg::Group* createBorder();
	osg::ref_ptr<osg::Group> border;
	osg::Group* createGrid();
	osg::Vec4 gridColor;
	osg::Vec4 color;
	osg::ref_ptr<osg::Group> grid;
	int gridDensity;
	osg::Node* createAxis(const osg::Vec3& s, const osg::Vec3& e, int numReps,float scale);
	osg::Node* createLabel(const osg::Vec3& pos, float size, const std::string& label);
	osg::Group* createXAxis(float scale);
	osg::Group* createYAxis(float scale);
	osg::ref_ptr<osg::Group> xAxis;
	osg::ref_ptr<osg::Group> yAxis;
	int xNumReps,yNumReps;
	void repaint();
public:
	Chart(int x,int y,int width,int height);
	~Chart();
	//! dodaje serie danych o zadanym indexie
	void addChartSeries(ChartData* data);
	//! Pobiera calkowita ilosc klatek 
	int getFrameNumber();
	//! podbiera ilosc klatek na sekunde
	int getFPS();
	//! Odswieza polozenie wskaznikow
	void updatePointer(double targetTime);
	//! Zaokragla podana liczbe do 2 miejsc po przecinku
	std::string formatNumber( float number );
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
	//! Funkcja pobierajaca ilosc przedzialek na osi X
	int getXNumReps();
	//! Funkcja ustawiajaca ilosc przedzialek na osi X
	void setXNumReps(int xNumReps);
	//! Funkcja pobierajaca ilosc przedzialek na osi Y
	int getYNumReps();
	//! Funkcja ustawiajaca ilosc przedzialek na osi Y
	void setYNumReps(int xNumReps);
	//! Funkcja ustawiajaca lokacje wykresu
	void setLocation(int x,int y,int width,int height);
	//! Funkcja pobierajaca lokacje wykresu
	osg::Vec4 getLocation();


};


#endif