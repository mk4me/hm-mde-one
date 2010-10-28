/********************************************************************
created:  2010/08/02
filename: Pointer.h
author:	  Michal Szafarski

purpose:  Klasa ta sluzy do rysowania wskaznika do wykresu
*********************************************************************/
#ifndef __HEADER_GUARD__POINTERUPDATECALLBACK_H__
#define __HEADER_GUARD__POINTERUPDATECALLBACK_H__

#include <osg/Geode>
#include <osg/Geometry>
#include <iostream>
#include <osg/MatrixTransform>
#include <osgText/Text>
#include <sstream>
#include "ChartData.h"

class PointerUpdateCallback : public osg::NodeCallback{
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
private:
	float actualPoint;
	float endPoint;
	osg::ref_ptr<osgText::Text> text;
	std::string float2str(float);
	ChartData* data;
public:
	PointerUpdateCallback(osgText::Text* text,ChartData* data,float endPoint);
};

#endif