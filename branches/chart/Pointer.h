/********************************************************************
created:  2010/08/02
filename: Pointer.h
author:	  Michal Szafarski

purpose:  Klasa ta sluzy do rysowania wskaznika do wykresu
*********************************************************************/
#ifndef __HEADER_GUARD__POINTER_H__
#define __HEADER_GUARD__POINTER_H__

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osgAnimation/BasicAnimationManager>
#include <osgAnimation/Channel>
#include <osgAnimation/UpdateMatrixTransform>
#include <osgAnimation/StackedTranslateElement>
#include "ChartData.h"
#include <osgText/Text>
#include "PointerUpdateCallback.h"

class Pointer : public osg::Group{
private:
	osg::Vec3 startPoint;
	osg::Vec3 endPoint;
	float time;
	osgText::Text* createLabel(const osg::Vec3& pos, float size, const std::string& label);
	osg::Geode* pointer;

public:
	Pointer(osg::Vec3 startPoint,osg::Vec3 endPoint,ChartData* data);
	osg::Geode* createPointer();

};


#endif