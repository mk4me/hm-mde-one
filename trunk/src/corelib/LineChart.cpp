#include <core/LineChart.h>
#include <core/ChartData.h>

#include <core/ChartPointer.h>

LineChart::LineChart(ChartData* data,int x,int y,int width, int height,osg::Vec4 color){
	this->x=x;
	this->y=y;
	this->width=width;
	this->height=height;
	this->data=data;
	LineChart::chartVertices=new osg::Vec3Array();
	setColor(color);
	for(int i=0;i<data->getRNumber();i++)
	{
		LineChart::chartVertices->push_back(osg::Vec3(data->getNormalizedXValue(i)*(width-x)+x,data->getNormalizedYValue(i)*(height-y)+y,-0.1));
	}
	
	chart=ChartDataSeries::drawChart(chartVertices); 
	

	pointer=new ChartPointer(osg::Vec3(x,y,0),osg::Vec3(width,height,0),data);
	this->addChild(pointer);
	this->addChild(chart);
	
}
void LineChart::addCoord(osg::Vec3 coord){
LineChart::chartVertices->push_back(osg::Vec3(coord));

}
ChartPointer* LineChart::getPointer(){
return pointer;
}
void LineChart::repaint(ChartData* data,int x,int y,int width,int height){
		this->x=x;
	this->y=y;
	this->width=width;
	this->height=height;
	this->data=data;
LineChart::chartVertices=new osg::Vec3Array();
	for(int i=0;i<data->getRNumber();i++)
	{
		LineChart::chartVertices->push_back(osg::Vec3(data->getNormalizedXValue(i)*(width-x)+x,data->getNormalizedYValue(i)*(height-y)+y,-0.1f));
	}
	
	osg::Geode* newChart=ChartDataSeries::drawChart(chartVertices); 
	this->replaceChild(chart,newChart);
chart=newChart;
	ChartPointer* newPointer=new ChartPointer(osg::Vec3(x,y,0),osg::Vec3(width,height,0),data);
	this->replaceChild(pointer,newPointer);
	pointer=newPointer;
}