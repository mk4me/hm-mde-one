#include "PCH.h"
#include <osg/LineWidth>
#include "uniqueCollection.h"
#include "TrajectoriesDrawer.h"

void TrajectoryDrawer::init( SkeletalVisualizationSchemeConstPtr scheme )
{
	UTILS_ASSERT(scheme && scheme->hasMarkers());
	OsgSchemeDrawer::init(scheme);

	node = new osg::Group;
	createTrajectories(scheme->getMarkers());
}

void TrajectoryDrawer::deinit()
{

}

void TrajectoryDrawer::draw()
{

}

void TrajectoryDrawer::update()
{

}

osg::ref_ptr<osg::Node> TrajectoryDrawer::getNode()
{
	return node;
}


TrajectoryDrawer::TrajectoryDrawer( const osg::Vec4& color, int density )
{
	this->color = color;
	this->density = density;
}


void TrajectoryDrawer::createTrajectories( MarkerCollectionConstPtr markers )
{
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	colors->push_back(color);
	
	

	for (int i = markers->getNumChannels() - 1; i >= 0; --i) {
		MarkerChannelConstPtr channel = markers->getChannel(i);

		// stateset colowo jest generowany dla kazdej trajektorii osobno
		// mozna w ten sposob zmieniac grubosc linii, itp.
		osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
		stateset->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
		stateset->setMode( GL_BLEND, osg::StateAttribute::ON );
		stateset->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
		GeodePtr geode = new osg::Geode();
		GeometryPtr lines = new osg::Geometry();
		osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;

		float mult = channel->getLength() / static_cast<float>(density);
		
		for (int n = 0; n < density - 1; n++) {	
			vertices->push_back((*channel)[mult * n]);
			vertices->push_back((*channel)[mult * (n + 1)]);
		}

		geode->setStateSet(stateset);
		lines->setVertexArray(vertices);
		lines->setColorArray(colors);
		lines->setColorBinding(osg::Geometry::BIND_OVERALL);
		lines->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, vertices->size()));

		trajectoriesMap[channel->getName()] = geode;
		geode->addDrawable(lines);
		
		node->addChild(geode);
		geode->setNodeMask(0);
	}
}

void TrajectoryDrawer::setVisible( const std::string& name, bool visible)
{
	GeodePtr ptr = trajectoriesMap[name];
	ptr->setNodeMask(visible ? 0xFFFF : 0);
}

osg::Vec4 TrajectoryDrawer::getColor( const std::string& name )
{
	auto it = trajectoriesMap.find(name);

	if (it != trajectoriesMap.end()) {
		GeodePtr ptr = it->second;
		const osg::Geometry* lines = dynamic_cast<const osg::Geometry*>(ptr->getDrawable(0));
		if (lines && lines->getColorArray() && lines->getColorArray()->getDataSize()) {
			const osg::Vec4Array* colors = static_cast<const osg::Vec4Array*>( lines->getColorArray() );
			return (*colors)[0];
		} 
	}

	throw std::runtime_error("Wrong color buffer");
}

void TrajectoryDrawer::setColor( const std::string& name, const osg::Vec4& color )
{
	GeodePtr ptr = trajectoriesMap[name];
	osg::Geometry* lines = dynamic_cast<osg::Geometry*>(const_cast<osg::Drawable*>(ptr->getDrawable(0)));
	if (lines) {
		osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
		colors->push_back(color);
		return lines->setColorArray(colors);
	} else {
		throw std::runtime_error("Wrong color buffer");
	}
}

void TrajectoryDrawer::setLineWidth( const std::string& name, float width )
{
	GeodePtr ptr = trajectoriesMap[name];
	osg::StateSet* stateset = ptr->getStateSet();

	osg::ref_ptr<osg::LineWidth> linewidth = new osg::LineWidth();
	linewidth->setWidth(width);
	stateset->setAttributeAndModes(linewidth,osg::StateAttribute::ON);
	//ptr->setStateSet(stateset);
}
