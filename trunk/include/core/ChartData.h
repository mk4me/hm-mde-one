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
	~ChartData();
	float getValue(int frame);
	float getNormalizedXValue(int frame);
	float getNormalizedYValue(int frame);
	void setMin();
	void setMax();
	float getScaleX();
	float getScaleY();
	void setScaleX(float scaleX);
	void setScaleY(float scaleY);
	int getRNumber();
	void setRNumber(int number);
	float getXMin();
	float getYMin();
	float getXMax();
	float getYMax();
	int getFPS();
	void setFPS(int FPS);
	void normalize();
	std::vector<float> getX();
	std::vector<float> getY();
	void setX(std::vector<float> x);
	void setY(std::vector<float> y);
	std::string getName();
	void setName(std::string name);
	void setScale(float scale);
	void setOffset(float offset);
	void setDescription(std::string description);
	void setUnit(std::string unit);
	std::string getUnit();
private:
	std::vector<float>::iterator itX;
	std::vector<float>::iterator itY;
	int rNumber;
	float yScale,xScale;
	std::vector<float> x;
	std::vector<float> y;
	std::vector<float> normalizedX;
	std::vector<float> normalizedY;
	float xMax,xMin,yMax,yMin;
	std::string name;
	float scale;
	float offset;
	std::string description;
	std::string unit;

	int FPS;
};
#endif