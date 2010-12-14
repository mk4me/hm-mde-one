#include <plugins/chart/ChartPCH.h>

#include <core/ChartData.h>
#include <core/c3dParser.h>
#include <core/C3D_Data.h>
#include <plugins/chart/Data.h>



ChartData::~ChartData(){}
void ChartData::normalize(){
	//std::vector<float>::iterator it= data.end()-1;


	for(int i=0;i<ChartData::rNumber;i++){
		normalizedY.push_back((y[i]-yMin)/yScale);
		normalizedX.push_back((x[i]-xMin)/xScale);
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
		if(frame>rNumber)
			return ChartData::normalizedY[rNumber-1];
		else
	return ChartData::normalizedY[frame];

}


float ChartData::getNormalizedXValue(int frame){
	if(frame>rNumber)
	return ChartData::normalizedX[rNumber-1];
	else
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
float ChartData::getXMax(){
	return xMax;
}
float ChartData::getYMax(){
	return yMax;
}


int ChartData::getFPS(){
 return FPS;
}
void ChartData::setFPS(int FPS){
this->FPS=FPS;
}
std::vector<float> ChartData::getX(){
return x;
}
std::vector<float> ChartData::getY(){
return y;
}
void ChartData::setX(std::vector<float> x){
this->x=x;
}
void ChartData::setY(std::vector<float> y){
this->y=y;
}
void ChartData::setScaleX(float scaleX){
	this->xScale=scaleX;
}
void ChartData::setScaleY(float scaleY){
	this->yScale=scaleY;

}

void ChartData::setName(std::string name){
this->name=name;
}

std::string ChartData::getName(){
return name;
}

void ChartData::setScale(float scale){
	this->scale=scale;
}
void ChartData::setOffset(float scale){
	this->scale=scale;
}
void ChartData::setDescription(std::string description){
	this->description=description;
}
void ChartData::setUnit(std::string unit){
	this->unit=unit;
}

std::string ChartData::getUnit(){
return unit;
}