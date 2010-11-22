#include "LineChart.h"


LineChart::LineChart(ChartData* data,int x,int y,int width, int height){
	this->x=x;
	this->y=y;
	this->width=width;
	this->height=height;
	this->data=data;
	LineChart::chartVertices=new osg::Vec3Array();

	for(int i=0;i<data->getRNumber();i++)
	{
		LineChart::chartVertices->push_back(osg::Vec3(data->getNormalizedXValue(i)*(width-x)+x,data->getNormalizedYValue(i)*(height-y)+y,0));
	}
	
	chart=ChartDataSeries::drawChart(chartVertices); 
	this->addChild(chart);

	pointer=new Pointer(osg::Vec3(x,y,0),osg::Vec3(width,height,0),data);
	this->addChild(pointer);
	
}
void LineChart::addCoord(osg::Vec3 coord){
LineChart::chartVertices->push_back(osg::Vec3(coord));

}
Pointer* LineChart::getPointer(){
return pointer;
}
void LineChart::repaint(int x,int y,int width,int height){
		this->x=x;
	this->y=y;
	this->width=width;
	this->height=height;
LineChart::chartVertices=new osg::Vec3Array();
	for(int i=0;i<data->getRNumber();i++)
	{
		LineChart::chartVertices->push_back(osg::Vec3(data->getNormalizedXValue(i)*(width-x)+x,data->getNormalizedYValue(i)*(height-y)+y,0));
	}
	
	osg::Geode* newChart=ChartDataSeries::drawChart(chartVertices); 
	this->replaceChild(chart,newChart);
chart=newChart;
	Pointer* newPointer=new Pointer(osg::Vec3(x,y,0),osg::Vec3(width,height,0),data);
	this->replaceChild(pointer,newPointer);
	pointer=newPointer;
}