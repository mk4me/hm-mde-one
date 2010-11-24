/********************************************************************
	created:  2010/09/20
	filename: ChartData.h
	author:	  Michal Szafarski
	
	purpose:  Klasa ta sluzy do przechowywania oraz pobierania danych
*********************************************************************/
#ifndef __HEADER_GUARD__CHARTDATA_H__
#define __HEADER_GUARD__CHARTDATA_H__



#include <string>

class Data;
class C3D_Data;
class c3dParser;


class ChartData {
public:
	
	//ChartData(std::string c3dFile,int chanel);
    float getValue(int frame);
	float getNormalizedXValue(int frame);
	float getNormalizedYValue(int frame);
	void setMin();
	void setMax();
	float getScaleX();
	float getScaleY();
	int getRNumber();
	void setRNumber(int number);
	float getXMin();
	float getYMin();
	int getFPS();
	void setFPS(int FPS);
		void normalize();
		float* getX();
		float* getY();
		void setX(float* x);
		void setY(float* y);
private:
	int rNumber;
	float yScale,xScale;
	float offset;
	float *x;
	float *y;
	float *normalizedX;
	float *normalizedY;
	float xMax,xMin,yMax,yMin;
	std::string description;


	//void exportAnalogChannel(int numberOfChannel, C3D_Data* c3dd);
	int FPS;
};
#endif