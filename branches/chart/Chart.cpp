#include "Chart.h"
#include "ChartWidget.h"

Chart::Chart(ChartWidget* chartWidget,std::string c3dFile){
	this->chartWidget=chartWidget;
	this->c3dFile=c3dFile;
	chartCount=0;
	chartDecorationCount=0;
	chartDecoration= new ChartDecoration*[chartDecorationCount];
	data= new ChartData*[chartCount];
	chart= new LineChart*[chartCount];
}
Chart::~Chart(){

}

LineChart* Chart::getChart(int index){

	return chart[index];
}
ChartData* Chart::getData(int index){
return data[index];
}
ChartDecoration* Chart::getDecoration(int index){
return chartDecoration[index];
}
void Chart::addChartDecoration(int index){
	ChartData** newData= new ChartData*[chartCount+1];
	ChartDecoration** newChartDecoration= new ChartDecoration*[chartDecorationCount+1];
	LineChart** newChart=new LineChart*[chartCount+1];
	for(int i=0;i<chartCount;i++){
		newData[i]=data[i];
		newChart[i]=chart[i];
	}
	for(int i=0;i<chartDecorationCount;i++){
		newChartDecoration[i]=chartDecoration[i];

	}
	newData[chartCount]= new ChartData(c3dFile,index);
	newChartDecoration[chartDecorationCount]= new ChartDecoration(osg::Vec3(20,20,0),500,250,newData[chartCount]);
	newChart[chartCount]=new LineChart(newData[chartCount],newChartDecoration[chartDecorationCount]);
	this->addChild(newChartDecoration[chartDecorationCount]);
	this->addChild(newChart[chartCount]);

	data=newData;
	chartDecoration=newChartDecoration;
	chart=newChart;
	chartCount++;
	chartDecorationCount++;
}

int Chart::getChartCount(){
return chartCount;
}
void Chart::setChartCount(int chartCount){
	this->chartCount=chartCount;
}
int Chart::getChartDecorationCount(){
return chartDecorationCount;
}
void Chart::setChartDecorationCount(int chartDecorationCount){
	this->chartDecorationCount=chartDecorationCount;
}
int Chart::getFrameNumber(int chartIndex){
	return data[chartIndex]->getRNumber();
}
int Chart::getFPS(int chartIndex){
	return data[chartIndex]->getFPS();
}
void Chart::updatePointer(double targetTime){
	for(int i=0;i<chartCount;i++)
		chart[i]->getPointer()->update(targetTime);
}

int Chart::getXNumReps(int chartDecorationIndex){
	return chartDecoration[chartDecorationIndex]->getXNumReps();
}
void Chart::setXNumReps(int chartDecorationIndex, int xNumReps){
	chartDecoration[chartDecorationIndex]->setXNumReps(xNumReps);
	chartDecoration[chartDecorationIndex]->repaint();
}
int Chart::getYNumReps(int chartDecorationIndex){
	return chartDecoration[chartDecorationIndex]->getYNumReps();
}
void Chart::setYNumReps(int chartDecorationIndex, int yNumReps){
	chartDecoration[chartDecorationIndex]->setXNumReps(yNumReps);
	chartDecoration[chartDecorationIndex]->repaint();
}

float Chart::getWidth(int chartDecorationIndex){
	return chartDecoration[chartDecorationIndex]->getXEnd();
}
void Chart::setWidth(int chartDecorationIndex, float width){
	chartDecoration[chartDecorationIndex]->setXEnd(width);
	chartDecoration[chartDecorationIndex]->repaint();
}
float Chart::getHeight(int chartDecorationIndex){
	return chartDecoration[chartDecorationIndex]->getYEnd();
}
void Chart::setHeight(int chartDecorationIndex, float height){
	chartDecoration[chartDecorationIndex]->setYEnd(height);
	chartDecoration[chartDecorationIndex]->repaint();
}

int Chart::getBorderSize(int chartDecorationIndex){
	return chartDecoration[chartDecorationIndex]->getBorderSize();
}
void Chart::setBorderSize(int chartDecorationIndex, int size){
	chartDecoration[chartDecorationIndex]->setBorderSize(size);
	chartDecoration[chartDecorationIndex]->repaint();
}

int Chart::getFontSize(int chartDecorationIndex){
	return chartDecoration[chartDecorationIndex]->getFontSize();
}
void Chart::setFontSize(int chartDecorationIndex, int size){
	chartDecoration[chartDecorationIndex]->setFontSize(size);
	chartDecoration[chartDecorationIndex]->repaint();
}

int Chart::getGridDensity(int chartDecorationIndex){
	return chartDecoration[chartDecorationIndex]->getGridDensity();
}
void Chart::setGridDensity(int chartDecorationIndex, int density){
	chartDecoration[chartDecorationIndex]->setGridDensity(density);
	chartDecoration[chartDecorationIndex]->repaint();
}

osg::Vec4 Chart::getGridColor(int chartDecorationIndex){
	return chartDecoration[chartDecorationIndex]->getGridColor();
}
void Chart::setGridColor(int chartDecorationIndex, osg::Vec4 color){
	chartDecoration[chartDecorationIndex]->setGridColor(color);
	chartDecoration[chartDecorationIndex]->repaint();
}

osg::Vec4 Chart::getColor(int chartDecorationIndex){
	return chartDecoration[chartDecorationIndex]->getColor();
}
void Chart::setColor(int chartDecorationIndex, osg::Vec4 color){
	chartDecoration[chartDecorationIndex]->setColor(color);
	chartDecoration[chartDecorationIndex]->repaint();
}
osg::Vec4 Chart::getLocation(int chartDecorationIndex){
	return osg::Vec4(chartDecoration[chartDecorationIndex]->getStartPoint().x(),
		chartDecoration[chartDecorationIndex]->getStartPoint().y(),getWidth(chartDecorationIndex)
		,getHeight(chartDecorationIndex));
}
void Chart::setLocation(int chartDecorationIndex,int x,int y,int width,int height){
	chartDecoration[chartDecorationIndex]->setLocation(x,y,width,height);
	for(int i=0;i<chartCount;i++)
		chart[i]->repaint();
	chartDecoration[chartDecorationIndex]->repaint();
}

void Chart::addChart(int chartDecorationIndex, int index){

	ChartData** newData= new ChartData*[chartCount+1];
	LineChart** newChart=new LineChart*[chartCount+1];
	for(int i=0;i<chartCount;i++){
		newData[i]=data[i];
		newChart[i]=chart[i];
	}

	newData[chartCount]= new ChartData(c3dFile,index);
	newChart[chartCount]=new LineChart(newData[chartCount],chartDecoration[chartDecorationIndex]);
	this->addChild(newChart[chartCount]);

	data=newData;
	chart=newChart;
	chartCount++;
}