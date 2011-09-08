#include "PCH.h"
#include "GRFSerie.h"

using namespace osg;
using namespace boost;
using namespace std;

osg::ref_ptr<osg::Group> GRFSerie::createPlatformsGroup( const c3dlib::ForcePlatformCollection& platforms )
{
	osg::ref_ptr<osg::Group> group = new osg::Group();
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(0.5f, 0.5f, 0.3f, 1.0f));
	osg::Vec3 up(0, 0, -0.00001f);
	int i = 0;
	for ( auto it = platforms.cbegin(); it != platforms.cend(); it++) {
		GeodePtr platformGeode = new osg::Geode();

		osg::ref_ptr<osg::Geometry> platformLines = new osg::Geometry();
		osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
		stateset->setMode( GL_LIGHTING, osg::StateAttribute::ON );
		platformGeode->setStateSet(stateset);

		osg::ref_ptr<osg::Box> box = new osg::Box((*it)->getCenter() - Vec3(0,0, 0.025f), (*it)->getWidth(), (*it)->getLength(), 0.025f);
		if (i == 0) {
			platform1 = new osg::ShapeDrawable(box);
			platform1->setColor(Vec4(0.5f, 0.5f, 0.3f, 1.0f));
			platformGeode->addDrawable(platform1.get());
		} else {
			platform2 = new osg::ShapeDrawable(box);
			platform2->setColor(Vec4(0.5f, 0.5f, 0.3f, 1.0f));
			platformGeode->addDrawable(platform2.get());
		}
		i++;
		group->addChild(platformGeode);
	}

	return group;
}

GRFSerie::GroupPtr GRFSerie::createButterfly( GRFCollectionConstPtr grf ) const
{
	osg::ref_ptr<osg::Group> group = new osg::Group();
		
	int numSegments = 300;
	float delta = (grf->getLength() / static_cast<float>(numSegments));
	GeodePtr geode = new osg::Geode();
	osg::ref_ptr<osg::Vec3Array> verts = new osg::Vec3Array;
	Vec3 v;
	osg::Vec3 origin = grf->getPlatforms()[0]->getCenter();
	osg::Vec3 origin2 = grf->getPlatforms()[1]->getCenter();

	osg::Vec4 highColor(1.0f, 0.0f, 0.0f, 0.7f);
	osg::Vec4 lowColor(0.0f, 1.0f, 0.0f, 0.1f);
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	//colors->push_back(osg::Vec4(1.0f, 0.3f, 0.3f, 1.0f));

	float f = 0.0f;
	float maxLength = f1->getLength();
	osg::Vec3 lastV1;
	osg::Vec3 lastV2;
	for (int i = 0; i < numSegments; i++) {
		v.set((*f1)[f]);
		v *= grfScale;
		v += origin;
		if (v.length() > 0.00001f) {
			colors->push_back(highColor);
			verts->push_back(v);
			colors->push_back(highColor);
			verts->push_back(lastV1);
			colors->push_back(lowColor);
			verts->push_back(origin); 
		}
		lastV1 = v;
		v.set((*f2)[f]);
		v *= grfScale;
		v += origin2;
		if (v.length() > 0.00001f) {
			colors->push_back(highColor);
			verts->push_back(v);
			colors->push_back(highColor);
			verts->push_back(lastV2);
			colors->push_back(lowColor);
			verts->push_back(origin2); 
		}
		lastV2 = v;
		f += delta;
		if (f > maxLength) {
			f = maxLength;
		}
		
	}

	osg::ref_ptr<osg::Geometry> lines = new osg::Geometry();

	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	stateset->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
	stateset->setMode( GL_BLEND, osg::StateAttribute::ON );
	stateset->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
	geode->setStateSet(stateset);
	lines->setVertexArray(verts);
	lines->setColorArray(colors);
	lines->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	lines->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES, 0, verts->size()));
	geode->addDrawable(lines);
	group->addChild(geode);
	return group;
}

GRFSerie::GroupPtr GRFSerie::createArrow()
{
	osg::ref_ptr<osg::Group> group = new osg::Group();
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(0.3f, 1.0f, 0.0f, 1.0f));

	GeodePtr arrowGeode1 = new osg::Geode();
	GeodePtr arrowGeode2 = new osg::Geode();
	arrowLines1 = new osg::Geometry();
	arrowLines2 = new osg::Geometry();
	arrow1 = new osg::Vec3Array(2);
	arrow2 = new osg::Vec3Array(2);

	Vec3 zero;
	(*arrow1)[0] = zero;
	(*arrow1)[1] = zero;
	(*arrow2)[0] = zero;
	(*arrow2)[1] = zero;

	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	stateset->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

	arrowGeode1->setStateSet(stateset);
	arrowLines1->setVertexArray(arrow1);
	arrowLines1->setColorArray(colors);
	arrowLines1->setColorBinding(osg::Geometry::BIND_OVERALL);
	arrowLines1->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, arrow1->size()));

	arrowGeode2->setStateSet(stateset);
	arrowLines2->setVertexArray(arrow2);
	arrowLines2->setColorArray(colors);
	arrowLines2->setColorBinding(osg::Geometry::BIND_OVERALL);
	arrowLines2->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, arrow2->size()));
	
	arrowGeode1->addDrawable(arrowLines1);
	arrowGeode2->addDrawable(arrowLines2);
	group->addChild(arrowGeode1);
	group->addChild(arrowGeode2);

	return group;
}
