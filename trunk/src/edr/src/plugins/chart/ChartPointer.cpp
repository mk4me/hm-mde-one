#include "ChartPointer.h"

#include "Chart.h"

//#include "ChartData.h"

ChartPointer::ChartPointer(osg::Vec3 startPoint,osg::Vec3 endPoint, ScalarChannelConstPtr data,bool labelVisable){
	this->startPoint=startPoint;
	this->endPoint=endPoint;
	this->data=data;
	this->labelVisable=labelVisable;

	
	
	trans = new osg::MatrixTransform(	);
	
	trans->setDataVariance(osg::Object::DYNAMIC);
	trans->setMatrix(osg::Matrix::identity());
	pointer=createPointer();
	trans->addChild (pointer);
	if(labelVisable){
	labelTrans = new osg::MatrixTransform(	);
	labelTrans->setDataVariance(osg::Object::DYNAMIC);
	labelTrans->setMatrix(osg::Matrix::identity());
	labelTrans->addChild(createLabelField());
	trans->addChild(labelTrans);}

	this->addChild(trans);

}
ChartPointer::~ChartPointer()
{

}
osg::Geode* ChartPointer::createPointer()
{
	osg::Vec3Array* vertices = new osg::Vec3Array;
	vertices->push_back(startPoint);
	vertices->push_back(osg::Vec3(startPoint.x(),endPoint.y(),1));

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

osgText::Text* ChartPointer::createLabel(const osg::Vec3& pos, float size, const std::string& label)
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

std::string formatNumber( float number )
{
 char s[100];
        sprintf(s,"%.2f",number);

  return s;
}
void ChartPointer::update(double targetTime)
{
	trans->setMatrix(osg::Matrix::translate(targetTime / data->getLength() * (endPoint.x()-startPoint.x()),0.0f,0.0f));
	if(labelVisable){
	labelTrans->setMatrix(osg::Matrix::translate(0.0f,data->getNormalizedValue(targetTime) * (endPoint.y()-startPoint.y()),0.0f));
	text->setText(formatNumber(data->getNormalizedValue(targetTime) - data->getMinValue()));}
}


osg::Geode* ChartPointer::createLabelField(){
	osg::Vec3Array* vertices = new osg::Vec3Array;
	vertices->push_back(osg::Vec3(startPoint.x(),startPoint.y(),0));
	vertices->push_back(osg::Vec3(startPoint.x()-40.0f,startPoint.y(),1.0f));
	vertices->push_back(osg::Vec3(startPoint.x()-40.0f,startPoint.y()-20,1.0f));
	vertices->push_back(osg::Vec3(startPoint.x(),startPoint.y()-20,1.0f));
	vertices->push_back(osg::Vec3(startPoint.x(),startPoint.y(),1.0f));
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


	//ss->setMode(GL_BLEND, osg::StateAttribute::ON);

	ss->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
	geode->addDrawable(geom);
	text=createLabel(osg::Vec3(startPoint.x()-20,startPoint.y()-10,0),10	,"");
	text->setDataVariance(osg::Object::DYNAMIC);
	
	geode->addDrawable(text);




	return geode;
}
