#include <plugins/chart/ChartPCH.h>
#include <core/LineChart.h>
#include <core/ChartData.h>

#include <core/ChartPointer.h>

LineChart::LineChart(ChartData* data,int x,int y,int width, int height,osg::Vec4 color){
	this->setName("data seires");
	this->x=x;
	this->y=y;
	this->width=width;
	this->height=height;
	this->data=data;
	LineChart::chartVertices=new osg::Vec3Array();
	setColor(color);
	float currentX=0;
	showLabel=true;
	for(int i=0;i<data->getRNumber();i++)
	{

		if(currentX<data->getNormalizedXValue(i+1)*(width-x))
		{
		LineChart::chartVertices->push_back(osg::Vec3(data->getNormalizedXValue(i)*(width-x)+x,data->getNormalizedYValue(i)*(height-y)+y,-0.1f));
		currentX=data->getNormalizedXValue(i)*(width-x)+2;	
		}
	}
	chart=ChartDataSeries::drawChart(chartVertices); 
	

	pointer=new ChartPointer(osg::Vec3(x,y,0),osg::Vec3(width,height,0),data,showLabel);
	this->addChild(pointer);
	this->addChild(chart);
	
}
LineChart::~LineChart(){};
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
	float currentX=0;
	for(int i=0;i<data->getRNumber();i++)
	{
	
		if(currentX<data->getNormalizedXValue(i+1)*(width-x)){
		LineChart::chartVertices->push_back(osg::Vec3(data->getNormalizedXValue(i)*(width-x)+x,data->getNormalizedYValue(i)*(height-y)+y,-0.1f));
		currentX=data->getNormalizedXValue(i)*(width-x)+2;
		}
		}
	
	osg::Geode* newChart=ChartDataSeries::drawChart(chartVertices); 
	this->replaceChild(chart,newChart);
	chart=newChart;
	ChartPointer* newPointer=new ChartPointer(osg::Vec3(x,y,0),osg::Vec3(width,height,0),data,showLabel);
	this->replaceChild(pointer,newPointer);
	pointer=newPointer;
}

void LineChart::setShowLabel(bool showLabel){
	this->showLabel=showLabel;
}