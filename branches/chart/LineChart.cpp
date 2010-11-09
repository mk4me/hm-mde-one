#include "LineChart.h"


LineChart::LineChart(ChartData* data,ChartDecoration* chartDecoration){
	LineChart::chartVertices=new osg::Vec3Array();
	for(int i=0;i<data->getRNumber();i++)
	{
		LineChart::chartVertices->push_back(osg::Vec3(data->getNormalizedXValue(i)*chartDecoration->getXEnd()+chartDecoration->getStartPoint().x(),data->getNormalizedYValue(i)*chartDecoration->getYEnd()+chartDecoration->getStartPoint().y(),0));
	}
	
	Chart::drawChart(chartVertices); 
	chartDecoration->setScaleX(data->getScaleX());
	chartDecoration->setScaleY(data->getScaleY());
	pointer=new Pointer(chartDecoration->getStartPoint(),chartDecoration->getEndPoint(),data);
	this->addChild(pointer);
	
}
void LineChart::addCoord(osg::Vec3 coord){
LineChart::chartVertices->push_back(osg::Vec3(coord));

}
Pointer* LineChart::getPointer(){
return pointer;
}