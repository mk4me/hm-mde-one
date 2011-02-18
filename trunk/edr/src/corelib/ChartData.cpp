#include <plugins/chart/ChartPCH.h>

#include <core/ChartData.h>
#include <core/c3dParser.h>
#include <core/C3D_Data.h>
#include <core/Data.h>
#include <core/C3DChannels.h>


deprecated_ChartData::~deprecated_ChartData(){}
void deprecated_ChartData::normalize(){
	//std::vector<float>::iterator it= data.end()-1;


	for(int i=0;i<deprecated_ChartData::rNumber;i++){
		normalizedY.push_back((y[i]-yMin)/yScale);
		normalizedX.push_back((x[i]-xMin)/xScale);
	}

}
void deprecated_ChartData::setMin(){
	xMin=x[0];
	yMin=y[0];
	for(int i=0;i<rNumber;i++){
		if(x[i]<xMin)
			xMin=x[i];
		if(y[i]<yMin)
			yMin=y[i];
	}

}

void deprecated_ChartData::setMax(){
	xMax=x[0];
	yMax=y[0];
	for(int i=0;i<rNumber;i++){
		if(x[i]>xMax)
			xMax=x[i];
		if(y[i]>yMax)
			yMax=y[i];
	}

}
float deprecated_ChartData::getValue(int frame){
	return deprecated_ChartData::y[frame];

}

float deprecated_ChartData::getNormalizedYValue(int frame){
		if(frame>rNumber)
			return deprecated_ChartData::normalizedY[rNumber-1];
		else
	return deprecated_ChartData::normalizedY[frame];

}


float deprecated_ChartData::getNormalizedXValue(int frame){
	if(frame>rNumber)
	return deprecated_ChartData::normalizedX[rNumber-1];
	else
	return deprecated_ChartData::normalizedX[frame];

}
float deprecated_ChartData::getScaleX(){return deprecated_ChartData::xScale;}
float deprecated_ChartData::getScaleY(){return deprecated_ChartData::yScale;}
int deprecated_ChartData::getRNumber(){return deprecated_ChartData::rNumber-1;}
void deprecated_ChartData::setRNumber(int rNumber){
	this->rNumber=rNumber;
}

float deprecated_ChartData::getXMin(){
	return xMin;
}
float deprecated_ChartData::getYMin(){
	return yMin;
}
float deprecated_ChartData::getXMax(){
	return xMax;
}
float deprecated_ChartData::getYMax(){
	return yMax;
}


int deprecated_ChartData::getFPS(){
 return FPS;
}
void deprecated_ChartData::setFPS(int FPS){
this->FPS=FPS;
}
std::vector<float> deprecated_ChartData::getX(){
return x;
}
std::vector<float> deprecated_ChartData::getY(){
return y;
}
void deprecated_ChartData::setX(std::vector<float> x){
this->x=x;
}
void deprecated_ChartData::setY(std::vector<float> y){
this->y=y;
}
void deprecated_ChartData::setScaleX(float scaleX){
	this->xScale=scaleX;
}
void deprecated_ChartData::setScaleY(float scaleY){
	this->yScale=scaleY;

}

void deprecated_ChartData::setName(std::string name){
this->name=name;
}

std::string deprecated_ChartData::getName(){
return name;
}

void deprecated_ChartData::setScale(float scale){
	this->scale=scale;
}
void deprecated_ChartData::setOffset(float offset){
	this->offset=offset;
}
void deprecated_ChartData::setDescription(std::string description){
	this->description=description;
}
void deprecated_ChartData::setUnit(std::string unit){
	this->unit=unit;
}

std::string deprecated_ChartData::getUnit(){
return unit;
}