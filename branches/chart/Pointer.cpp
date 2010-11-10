#include "Pointer.h"
#include <plugins/timeline/ITimeline.h>
#include <plugins/timeline/Stream.h>
#define TIMERMULTIPLAY 0.0055



Pointer::Pointer(osg::Vec3 startPoint,osg::Vec3 endPoint,ChartData* data){
	this->startPoint=startPoint;
	this->endPoint=endPoint;
	this->data=data;
	
	text=createLabel(osg::Vec3(startPoint.x(),endPoint.y()+20,0),10	,"");
	text->setDataVariance(osg::Object::DYNAMIC);
	osg::Geode* geode = new osg::Geode();
	osg::StateSet* stateset = geode->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
	geode->addDrawable( text );

	//time=data->getRNumber()/100;

	trans = new osg::MatrixTransform(	);
	trans->setName("AnimatedNode");
	trans->setDataVariance(osg::Object::DYNAMIC);
	trans->setMatrix(osg::Matrix::identity());
	//trans->addUpdateCallback(new PointerUpdateCallback(text,data,endPoint.x()-startPoint.x()));
	pointer=createPointer();
	trans->addChild (pointer);
	trans->addChild(geode);


	this->addChild(trans);

}

osg::Geode* Pointer::createPointer(){
	osg::Vec3Array* vertices = new osg::Vec3Array;
	vertices->push_back(startPoint);
	vertices->push_back(osg::Vec3(startPoint.x(),endPoint.y(),0));

	osg::Group* group = new osg::Group;
	osg::Geometry* geom = new osg::Geometry;
	osg::Vec4Array* colors=new osg::Vec4Array();
	colors->push_back(osg::Vec4(1.0,0.0,0.0,1.0));


	geom->setColorArray(colors);
	geom->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE);
	geom->setVertexArray(vertices);

	geom->addPrimitiveSet(new osg::DrawArrays(GL_LINE_STRIP,0,vertices->size()));

	osg::Geode* geode = new osg::Geode;
	osg::StateSet* ss = geode->getOrCreateStateSet();


	ss->setMode(GL_BLEND, osg::StateAttribute::ON);

	ss->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
	geode->addDrawable(geom);





	return geode;
}

osgText::Text* Pointer::createLabel(const osg::Vec3& pos, float size, const std::string& label)
{

	std::string timesFont("fonts/times.ttf");


	osgText::Text* text = new  osgText::Text;
	text->setPosition(pos);
	text->setColor(osg::Vec4(1.0,0.0,0.0,1.0));
	text->setFont(timesFont);
	text->setCharacterSize(size);
	text->setAlignment(osgText::Text::CENTER_CENTER);
	text->setText(label);




	return text;    
}

std::string float2str(float i)
{
	std::stringstream ss;
	std::string temp;
	ss << i;
	ss >> temp;
	return temp;
}
void Pointer::update(double targetTime){
	
	int frame=targetTime*data->getFPS();
	//if(frame<data->getRNumber()-1){
	trans->setMatrix(osg::Matrix::translate(data->getNormalizedXValue(frame) * (endPoint.x()-startPoint.x()),0.0f,0.0f));
	text->setText(float2str(data->getValue(frame)-data->getYMin()));
	//}
}