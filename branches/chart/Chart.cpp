#include "Chart.h"


osg::Geode* Chart::drawChart(osg::Vec3Array* chartVertices)
{

	colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(0.0f,1.0f,0.0f, 1.0f));

	osg::Geometry* geom = new osg::Geometry;
	geom->setVertexArray(chartVertices);
	geom->setColorArray(colors);
	geom->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE);
	geom->addPrimitiveSet(new osg::DrawArrays(GL_LINE_STRIP,0,chartVertices->size()));

	osg::Geode* geode = new osg::Geode;
	osg::StateSet* ss = geode->getOrCreateStateSet();

	ss->setMode(GL_BLEND, osg::StateAttribute::ON);
	ss->setMode(GL_LIGHTING,osg::StateAttribute::OFF);

	geode->addDrawable(geom);

	return geode;

}


void Chart::setColor(float r,float g, float b, float a){

	Chart::colors=new osg::Vec4Array;
	Chart::colors->push_back(osg::Vec4(r,g,b,a));

}