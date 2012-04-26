/********************************************************************
	created:	2012/02/22
	created:	22:2:2012   10:13
	filename: 	Scales.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__SCALES_H__
#define HEADER_GUARD_NEW_CHART__SCALES_H__

#include <limits>

class Scales
{
public:
    Scales() : 
	  xMin((std::numeric_limits<float>::max)()),
      xMax((std::numeric_limits<float>::min)()),
      yMin((std::numeric_limits<float>::max)()),
      yMax((std::numeric_limits<float>::min)()),
      initialized(false) 
    {}
    Scales(float xMin, float xMax, float yMin, float yMax) :
      xMin(xMin),
      xMax(xMax),
      yMin(yMin),
      yMax(yMax),
      initialized(true) 
    {}

public:
    float getYMax() const { return yMax; }
    float getYMin() const { return yMin; }
    float getXMax() const { return xMax; }
    float getXMin() const { return xMin; }
    bool isInitialized() const { return initialized; }

public:
    void clear() { initialized = false; }
    void merge(const Scales& scales);

private:
    float xMin, xMax, yMin, yMax;
    bool initialized;
};



#endif
