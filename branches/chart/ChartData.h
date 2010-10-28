/********************************************************************
	created:  2010/09/20
	filename: ChartData.h
	author:	  Michal Szafarski
	
	purpose:  Klasa ta sluzy do przechowywania oraz pobierania danych
*********************************************************************/
#ifndef __HEADER_GUARD__CHARTDATA_H__
#define __HEADER_GUARD__CHARTDATA_H__


#include "c3dParser.h"
#include "C3D_Data.h"
#include "Data.h"
#include <fstream>
#include <iostream>
#include <sstream>




class ChartData {
public:
	
	ChartData(std::string c3dFile,int chanel);
    float getValue(int frame);
	float getNormalizedXValue(int frame);
	float getNormalizedYValue(int frame);
	void setMin();
	void setMax();
	float getScaleX();
	float getScaleY();
	int getRNumber();
	float getXMin();
	float getYMin();
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
	void normalize();
	void setRNumber(std::string c3dFile);
	void exportAnalogChannel(int numberOfChannel, C3D_Data* c3dd);
};
#endif