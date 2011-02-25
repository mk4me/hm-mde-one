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
#include <core/C3DChannels.h>
#include <core/ChartSerie.h>

class deprecated_LineChart;
class deprecated_ChartData;

////////////////////////////////////////////////////////////////////////////////
namespace osg {
////////////////////////////////////////////////////////////////////////////////

    typedef osg::ref_ptr<osg::Geode> GeodePtr;
    typedef osg::ref_ptr<osg::Geometry> GeometryPtr;

////////////////////////////////////////////////////////////////////////////////
} // namespace osg
////////////////////////////////////////////////////////////////////////////////

class Chart : public osg::Group
{
private:
    osg::GeodePtr background;
    osg::GeometryPtr grid;
    osg::GeometryPtr axisX;
    osg::GeometryPtr axisY;
    
    bool showGridX;
    bool showGridY;
    float margin;

private:

	int x,y,width,height;
	//std::vector<deprecated_LineChart*> deprecated_dataSeries;
	//std::vector<deprecated_ChartData*> deprecated_data;
    std::vector<core::ChartSeriePtr> series;
	std::vector<osg::Geode*> mainLabel;
	void init();
	//! odleglosc ramki od wykresu
	int fontSize;
	osg::Geometry* createLine(int x,int y,int x1,int y1,int z,osg::Vec4 lineColor);
	void refreshAxis();
	void refreshGrid();
	osg::Vec4 gridColor;
	osg::Vec4 color;
	//osg::ref_ptr<osg::Geode> grid;
	int gridDensity;
	osg::Geode* createAxis(const osg::Vec3& s, const osg::Vec3& e, int numReps,float scale,std::string unit);
	osgText::Text* createLabel(const osg::Vec3& pos, float size, const std::string& label);

	osg::ref_ptr<osg::Geode> xAxis;
	osg::ref_ptr<osg::Geode> yAxis;
	int xNumReps,yNumReps;
	void refresh();
	osg::Geode* createMainLabel(osg::Vec4 color,std::string name);
	int labelOffset;
	bool showLabel;
	bool showBorder;
public:
	Chart(int x,int y,int width,int height);
	~Chart();
// 	//! dodaje serie danych o zadanym indexie
// 	void deprecated_addChartSeries(deprecated_ChartData* data,osg::Vec4 color);
// /*    void addChartSeries(ScalarChannel* data,osg::Vec4 color);*/
// 	void deprecated_addChartPreviewSeries(deprecated_ChartData* data,osg::Vec4 color);
// 	//! Pobiera calkowita ilosc klatek 
// 	int deprecated_getFrameNumber();
// 	//! podbiera ilosc klatek na sekunde
// 	int deprecated_getFPS();

    void addChannel( const core::ScalarChannelPtr& channel, osg::Vec4 color );

	//! Odswieza polozenie wskaznikow
	void updatePointer(double targetTime);
	//! Zaokragla podana liczbe do 2 miejsc po przecinku
	std::string formatNumber( float number );

	//! zwraca odstep ramki od wykresu
	float getMargin() const;
	//! ustawia odstep ramki od wykres
	void setMargin(float margin);

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

    //! \return D³ugoœæ.
    float getLength() const;

	void setShowLabel(bool showLabel);
	void setShowBorder(bool showBorder);
	bool isShowingBorder();

private:
    bool prepareGeometry(osg::GeometryPtr& geom, bool condition, const char* name);
};


#endif