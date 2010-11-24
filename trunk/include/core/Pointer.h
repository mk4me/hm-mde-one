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


#include <osgText/Text>

class ChartData;

class Pointer : public osg::Group{
private:
	osg::Vec3 startPoint;
	osg::Vec3 endPoint;
	osgText::Text* createLabel(const osg::Vec3& pos, float size, const std::string& label);
	osg::Geode* pointer;
	osg::ref_ptr<osg::MatrixTransform> trans;

	ChartData* data;
	osg::ref_ptr<osgText::Text> text;

public:
	Pointer(osg::Vec3 startPoint,osg::Vec3 endPoint,ChartData* data);
	osg::Geode* createPointer();

	void update(double targetTime);
};


#endif