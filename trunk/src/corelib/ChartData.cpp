#include <core/ChartData.h>
#include <core/c3dParser.h>
#include <core/C3D_Data.h>
#include <plugins/chart/Data.h>
#include <plugins/chart/ChartPCH.h>



void ChartData::normalize(){
			normalizedY=new float[getRNumber()];
		normalizedX=new float[getRNumber()];
	ChartData::setMax();
	ChartData::setMin();
	ChartData::yScale=ChartData::yMax-ChartData::yMin;
	ChartData::xScale=ChartData::xMax-ChartData::xMin;
	for(int i=0;i<ChartData::rNumber;i++){
		normalizedY[i]=(y[i]-yMin)/yScale;
		normalizedX[i]=(x[i]-xMin)/xScale;
	}

}
void ChartData::setMin(){
	xMin=x[0];
	yMin=y[0];
	for(int i=0;i<rNumber;i++){
		if(x[i]<xMin)
			xMin=x[i];
		if(y[i]<yMin)
			yMin=y[i];
	}

}

void ChartData::setMax(){
	xMax=x[0];
	yMax=y[0];
	for(int i=0;i<rNumber;i++){
		if(x[i]>xMax)
			xMax=x[i];
		if(y[i]>yMax)
			yMax=y[i];
	}

}
float ChartData::getValue(int frame){
	return ChartData::y[frame];

}

float ChartData::getNormalizedYValue(int frame){
	return ChartData::normalizedY[frame];

}


float ChartData::getNormalizedXValue(int frame){
	return ChartData::normalizedX[frame];

}
float ChartData::getScaleX(){return ChartData::xScale;}
float ChartData::getScaleY(){return ChartData::yScale;}
int ChartData::getRNumber(){return ChartData::rNumber-1;}
void ChartData::setRNumber(int rNumber){
	this->rNumber=rNumber;
}

float ChartData::getXMin(){
	return xMin;
}
float ChartData::getYMin(){
	return yMin;
}


int ChartData::getFPS(){
 return FPS;
}
void ChartData::setFPS(int FPS){
this->FPS=FPS;
}
float* ChartData::getX(){
return x;
}
float* ChartData::getY(){
return y;
}
void ChartData::setX(float* x){
this->x=x;
}
void ChartData::setY(float* y){
this->y=y;
}