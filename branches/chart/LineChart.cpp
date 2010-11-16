#include "LineChart.h"


LineChart::LineChart(ChartData* data,ChartDecoration* chartDecoration){
	this->chartDecoration=chartDecoration;
	this->data=data;
	LineChart::chartVertices=new osg::Vec3Array();
	for(int i=0;i<data->getRNumber();i++)
	{
		LineChart::chartVertices->push_back(osg::Vec3(data->getNormalizedXValue(i)*chartDecoration->getXEnd()+chartDecoration->getStartPoint().x(),data->getNormalizedYValue(i)*chartDecoration->getYEnd()+chartDecoration->getStartPoint().y(),0));
	}
	
	chart=Chart::drawChart(chartVertices); 
	this->addChild(chart);

	pointer=new Pointer(chartDecoration->getStartPoint(),chartDecoration->getEndPoint(),data);
	this->addChild(pointer);
	
}
void LineChart::addCoord(osg::Vec3 coord){
LineChart::chartVertices->push_back(osg::Vec3(coord));

}
Pointer* LineChart::getPointer(){
return pointer;
}
void LineChart::repaint(){
LineChart::chartVertices=new osg::Vec3Array();
	for(int i=0;i<data->getRNumber();i++)
	{
		LineChart::chartVertices->push_back(osg::Vec3(data->getNormalizedXValue(i)*chartDecoration->getXEnd()+chartDecoration->getStartPoint().x(),data->getNormalizedYValue(i)*chartDecoration->getYEnd()+chartDecoration->getStartPoint().y(),0));
	}
	
	osg::Geode* newChart=Chart::drawChart(chartVertices); 
	this->replaceChild(chart,newChart);
chart=newChart;
	Pointer* newPointer=new Pointer(chartDecoration->getStartPoint(),chartDecoration->getEndPoint(),data);
	this->replaceChild(pointer,newPointer);
	pointer=newPointer;
}