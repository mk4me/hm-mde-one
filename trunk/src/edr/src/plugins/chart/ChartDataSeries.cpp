#include "ChartPCH.h"
#include "ChartDataSeries.h"


osg::Geode* ChartDataSeries::drawChart(osg::Vec3Array* chartVertices)
{
	osg::Geometry* geom = new osg::Geometry;
	geom->setVertexArray(chartVertices);
	if(colors){
	    geom->setColorArray(colors);
	    geom->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE);
    }
	geom->addPrimitiveSet(new osg::DrawArrays(GL_LINE_STRIP,0,chartVertices->size()));
    
	osg::Geode* geode = new osg::Geode;
	geode->addDrawable(geom);

	return geode;
}


void ChartDataSeries::setColor(osg::Vec4 color){
	this->color=color;
	colors = new osg::Vec4Array;
	ChartDataSeries::colors->push_back(color);
}

osg::Vec4 ChartDataSeries::getColor()
{
	return color;
}
