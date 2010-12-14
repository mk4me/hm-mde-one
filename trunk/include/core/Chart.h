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
	std::vector<osg::Geode*> mainLabel;
	void init();
	//! odleglosc ramki od wykresu
	int borderSize,fontSize;
	osg::Geometry* createLine(int x,int y,int x1,int y1,int z,osg::Vec4 lineColor);
	osg::Geode* createBorder();
	osg::ref_ptr<osg::Geode> border;
	osg::Geode* createGrid();
	osg::Vec4 gridColor;
	osg::Vec4 color;
	osg::ref_ptr<osg::Geode> grid;
	int gridDensity;
	osg::Geode* createAxis(const osg::Vec3& s, const osg::Vec3& e, int numReps,float scale,std::string unit);
	osgText::Text* createLabel(const osg::Vec3& pos, float size, const std::string& label);

	osg::ref_ptr<osg::Geode> xAxis;
	osg::ref_ptr<osg::Geode> yAxis;
	int xNumReps,yNumReps;
	void repaint();
	osg::Geode* createMainLabel(osg::Vec4 color,std::string name);
	int labelOffset;
	bool labelVisable;
public:
	Chart(int x,int y,int width,int height);
	~Chart();
	//! dodaje serie danych o zadanym indexie
	void addChartSeries(ChartData* data,osg::Vec4 color);
	void addChartPreviewSeries(ChartData* data,osg::Vec4 color);
	//! Pobiera calkowita ilosc klatek 
	int getFrameNumber();
	//! podbiera ilosc klatek na sekunde
	int getFPS();
	//! Odswieza polozenie wskaznikow
	void updatePointer(double targetTime);
	//! Zaokragla podana liczbe do 2 miejsc po przecinku
	std::string formatNumber( float number );
	//! zwraca odstep ramki od wykresu
	int getBorderSize();
	//! ustawia odstep ramki od wykres
	void setBorderSize(int borderSize);
	//! zwraca wielkosc czcionki
	int getFontSize();
	//! ustawia wielkosc czcionki 
	void setFontSize(int fontSize);
	//! pobiera gestosc siatki. Ile pixeli szerokosci ma jedna kratka
	int getGridDensity();
	//! ustawia gestosc siatki. Ile pixeli szerokosci ma jedna kratka
	void setGridDensity(int gridDensity);
	//! pobiera kolor siatki w rgba
	osg::Vec4 getGridColor();
	//! ustawia kolor siatki w rgba
	void setGridColor(osg::Vec4 gridColor);
	//! pobiera kolor wykresu w rgba
	osg::Vec4 getColor();
	//! ustawia kolor wykresu w rgba
	void setColor(osg::Vec4 color);
	//! pobiera ilosc przedzialek na osi X
	int getXNumReps();
	//! ustawia ilosc przedzialek na osi X
	void setXNumReps(int xNumReps);
	//! pobiera ilosc przedzialek na osi Y
	int getYNumReps();
	//! ustawia ilosc przedzialek na osi Y
	void setYNumReps(int xNumReps);
	//! ustawia lokacje wykresu
	void setLocation(int x,int y,int width,int height);
	//! pobiera lokacje wykresu
	osg::Vec4 getLocation();

	void setLabelVisable(bool labelVisable);
};


#endif