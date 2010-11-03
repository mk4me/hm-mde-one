#include "Pointer.h"





Pointer::Pointer(osg::Vec3 startPoint,osg::Vec3 endPoint,ChartData* data){
	this->startPoint=startPoint;
	this->endPoint=endPoint;
	osg::ref_ptr<osgText::Text> text=createLabel(osg::Vec3(startPoint.x(),endPoint.y()+20,0),10	,"");
	text->setDataVariance(osg::Object::DYNAMIC);
	osg::Geode* geode = new osg::Geode();
	osg::StateSet* stateset = geode->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
	geode->addDrawable( text );

	time=data->getRNumber()/100;

	osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform(	);
	trans->setName("AnimatedNode");
	trans->setDataVariance(osg::Object::DYNAMIC);
	osgAnimation::UpdateMatrixTransform* updatecb = new osgAnimation::UpdateMatrixTransform("AnimatedCallback");
	updatecb->getStackedTransforms().push_back(new osgAnimation::StackedTranslateElement("position"));
	trans->setUpdateCallback(updatecb);
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
