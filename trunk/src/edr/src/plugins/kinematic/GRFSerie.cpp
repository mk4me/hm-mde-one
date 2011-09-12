#include "PCH.h"
#include "GRFSerie.h"

using namespace osg;
using namespace boost;
using namespace std;

const osg::Vec3 arrowScale(0.03f, 0.03f, 1.0f);
// prog dla ktorego sensowne jest wizualizowanie odczytow GRF
const float treshold = 0.01f;

osg::ref_ptr<osg::Group> GRFSerie::createPlatformsGroup( const c3dlib::ForcePlatformCollection& platforms )
{
	osg::ref_ptr<osg::Group> group = new osg::Group();
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(0.5f, 0.5f, 0.3f, 1.0f));
	osg::Vec3 up(0, 0, -0.00001f);
	int i = 0;
	for ( auto it = platforms.cbegin(); it != platforms.cend(); it++) {
		GeodePtr platformGeode = new osg::Geode();

		GeometryPtr platformLines = new osg::Geometry();
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

GRFSerie::GroupPtr GRFSerie::createButterfly( GRFCollectionConstPtr grf, float& maxLength ) const
{
	osg::ref_ptr<osg::Group> group = new osg::Group();

	osg::Vec3 start1 = f1->getStartPoint();
	osg::Vec3 end1   = f1->getEndPoint();
	osg::Vec3 start2 = f2->getStartPoint();
	osg::Vec3 end2   = f2->getEndPoint();
	float startTime1 = f1->getDataStartTime();
	float endTime1	 = f1->getDataEndTime();
	float startTime2 = f2->getDataStartTime();
	float endTime2	 = f2->getDataEndTime();

	int numSegments = 300;
	float delta = (grf->getLength() / static_cast<float>(numSegments));
	GeodePtr geode = new osg::Geode();
	osg::ref_ptr<osg::Vec3Array> verts = new osg::Vec3Array;
	Vec3 v;
	osg::Vec3 origin1 = grf->getPlatforms()[0]->getCenter();
	osg::Vec3 origin2 = grf->getPlatforms()[1]->getCenter();

	osg::Vec4 highColor(1.0f, 0.0f, 0.0f, 0.7f);
	osg::Vec4 lowColor(0.0f, 1.0f, 0.0f, 0.0f);
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	//colors->push_back(osg::Vec4(1.0f, 0.3f, 0.3f, 1.0f));

	maxLength = 0.0f;

	float f = 0.0f;
	float lengthBuffer = f1->getLength();
	osg::Vec3 lastV1;
	osg::Vec3 lastV2;
	osg::Vec3 lastOrigin1;
	osg::Vec3 lastOrigin2;
	for (int i = 0; i < numSegments; i++) {
		v.set((*f1)[f]);
		float length = v.length();

		float ratio1 = (f - startTime1) / (endTime1 - startTime1);
		float ratio2 = (f - startTime2) / (endTime2 - startTime2);
		origin1  = start1  * (1.0f - ratio1) + end1 * ratio1;
		origin2 = start2  * (1.0f - ratio2) + end2 * ratio2;
		v *= grfScale;	 
		v[2] *= -1.0f;
		v += origin1;
		
		//if (length > treshold) {
		if (ratio1 >= 0.0f && ratio1 <= 1.0f) {
			colors->push_back(highColor);
			verts->push_back(v);
			colors->push_back(highColor);
			verts->push_back(lastV1);
			colors->push_back(lowColor);
			verts->push_back(origin1); 

			colors->push_back(highColor);
			verts->push_back(lastV1);
			colors->push_back(lowColor);
			verts->push_back(origin1);
			colors->push_back(lowColor);
			verts->push_back(lastOrigin1);
			
		}
		if (length > maxLength) {
			maxLength = length;
		}
		lastV1 = v;
		lastOrigin1 = origin1;
		v.set((*f2)[f]);
		length = v.length();
		v *= grfScale;
		v[2] *= -1.0f;
		v += origin2;
		
		//if (length > treshold) {
		if (ratio2 >= 0.0f && ratio2 <= 1.0f) {
			colors->push_back(highColor);
			verts->push_back(v);
			colors->push_back(highColor);
			verts->push_back(lastV2);
			colors->push_back(lowColor);
			verts->push_back(origin2); 

			colors->push_back(highColor);
			verts->push_back(lastV2);
			colors->push_back(lowColor);
			verts->push_back(origin2);
			colors->push_back(lowColor);
			verts->push_back(lastOrigin2);
		}

		if (length > maxLength) {
			maxLength = length;
		}
		lastV2 = v;
		lastOrigin2 = origin2;
		f += delta;
		if (f > lengthBuffer) {
			f = lengthBuffer;
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

void GRFSerie::setTime( double time )
{
	try {
		float t = static_cast<float>(time);
		osg::Vec3 v1((*f1)[t]);
		osg::Vec3 v2((*f2)[t]);

		float ratio1 = v1.length() / maxLength;
		float ratio2 = v2.length() / maxLength;

		osg::Vec4 c1(ratio1, 1.0f - ratio1, 0.0f, 1.0f);
		osg::Vec4 c2(ratio2, 1.0f - ratio2, 0.0f, 1.0f);

		v1 *= grfScale;
		v1[2] *= -1.0f;
		v2 *= grfScale;
		v2[2] *= -1.0f;
		//osg::Vec3 origin1 = grfCollection->getPlatforms()[0]->getCenter();
		//osg::Vec3 origin2 = grfCollection->getPlatforms()[1]->getCenter();

		osg::Vec3 start1 = f1->getStartPoint();
		osg::Vec3 end1   = f1->getEndPoint();
		osg::Vec3 start2 = f2->getStartPoint();
		osg::Vec3 end2   = f2->getEndPoint();
		float startTime1 = f1->getDataStartTime();
		float endTime1	 = f1->getDataEndTime();
		float startTime2 = f2->getDataStartTime();
		float endTime2	 = f2->getDataEndTime();

		float timeRatio1 = (time - startTime1) / (endTime1 - startTime1);
		float timeRatio2 = (time - startTime2) / (endTime2 - startTime2);

		osg::Vec3 origin1  = start1  * (1.0f - timeRatio1) + end1 * timeRatio1;
		osg::Vec3 origin2 = start2  * (1.0f - timeRatio2) + end2 * timeRatio2;
		
		// chowanie/pokazywanie strzalek, kolorowanie platform.
		//if (v1.length2() > treshold) {
		if (timeRatio1 >= 0.0f && timeRatio1 <= 1.0f) {
			a1->mainPtr->setNodeMask(0xffff);
			a1->setArrow(origin1, v1 + origin1);	
			a1->setColor(c1);
			//g1->addState(std::make_pair(origin1, v1 + origin1));
			platform1->setColor(osg::Vec4(0.4f, 0.4f, 0.0f, 1.0f));
		} else {
			a1->mainPtr->setNodeMask(0);
			platform1->setColor(osg::Vec4(0.5f, 0.5f, 0.3f, 1.0f));
		}

		//if (v2.length2() > treshold) {
		if (timeRatio2 >= 0.0f && timeRatio2 <= 1.0f) {
			a2->setColor(c2);
			a2->setArrow(origin2, v2 + origin2);
			a2->mainPtr->setNodeMask(0xffff);
			//g2->addState(std::make_pair(origin2, v2 + origin2));
			platform2->setColor(osg::Vec4(0.4f, 0.4f, 0.0f, 1.0f));
		} else {
			a2->mainPtr->setNodeMask(0);
			platform2->setColor(osg::Vec4(0.5f, 0.5f, 0.3f, 1.0f));
		}

		/*g1->update();
		g2->update();*/
	} catch(...) {
	}
}

GRFSerie::ArrowPtr GRFSerie::createArrow()
{
	GeodePtr boxGeode = new osg::Geode();
	GeodePtr coneGeode = new osg::Geode();

	osg::ref_ptr<osg::Box> unitBox = new osg::Box(Vec3(0, 0, 0), 0.5f, 0.5f, 0.8f);
	osg::ref_ptr<osg::Cone> unitCone = new osg::Cone(Vec3(0, 0, 0), 1.0f, 0.2f);
	osg::ref_ptr<osg::ShapeDrawable> boxShape = new osg::ShapeDrawable(unitBox);
	osg::ref_ptr<osg::ShapeDrawable> coneShape = new osg::ShapeDrawable(unitCone);

	boxGeode->addDrawable(boxShape);
	coneGeode->addDrawable(coneShape);

	TransformPtr transformBox = new osg::PositionAttitudeTransform();
	TransformPtr transformCone = new osg::PositionAttitudeTransform();
	TransformPtr transform = new osg::PositionAttitudeTransform();

	transformBox->addChild(boxGeode);
	transformBox->setName("BOX");
	transformBox->setPosition(Vec3(0, 0, 0.4f));
	transformCone->addChild(coneGeode);
	transformCone->setName("CONE");
	transformCone->setPosition(Vec3(0, 0, 0.4f));

	transform->setScale(arrowScale);

	transform->addChild(transformBox);
	transformBox->addChild(transformCone);
	//transform->addChild(transformCone);

	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	stateset->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
	stateset->setMode( GL_BLEND, osg::StateAttribute::ON );
	stateset->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
	transform->setStateSet(stateset);

	ArrowPtr a(new Arrow);
	a->mainPtr = transform;
	a->conePtr = transformCone;
	a->boxPtr = transformBox;
	a->boxShape = boxShape;
	a->coneShape = coneShape;
	return a;
}

void GRFSerie::setSerieData( const core::ObjectWrapperConstPtr & data )
{
	UTILS_ASSERT(data->getTypeInfo() == typeid(GRFCollection));
	grfCollection = data->get();

	if (grfCollection->getPlatforms().size() == 2) {
		f1 = grfCollection->getGRFChannel(GRFChannel::F1);
		f2 = grfCollection->getGRFChannel(GRFChannel::F2);
		visualizer->transformNode->addChild(createPlatformsGroup(grfCollection->getPlatforms()));
		visualizer->transformNode->addChild(createButterfly(grfCollection, this->maxLength));

		a1 = createArrow();
		a2 = createArrow();

		a1->mainPtr->setNodeMask(0);
		a2->mainPtr->setNodeMask(0);

		visualizer->transformNode->addChild(a1->mainPtr);
		visualizer->transformNode->addChild(a2->mainPtr);
		GhostStackPtr ghost1(new GhostStack(8, visualizer->transformNode, osg::Vec4(1,1,1,1)));
		GhostStackPtr ghost2(new GhostStack(8, visualizer->transformNode, osg::Vec4(1,1,1,1)));
		g1 = ghost1;
		g2 = ghost2;
	} else {
		grfCollection.reset();
	}
}


void GRFSerie::Arrow::setArrow( Vec3 from, Vec3 to )
{
	Vec3 dir = to - from;
	Vec3 zero;
	Vec3 up(0.0f, 1.0f, 0.0f);
	float length = dir.normalize();
	osg::Matrix mat;
	osg::Quat rotation;
	mat.makeLookAt(zero, -dir, up);
	rotation.set(Matrix::inverse(mat));

	mainPtr->setScale(Vec3(arrowScale[0], arrowScale[1], arrowScale[2] * length));
	mainPtr->setAttitude(rotation);
	mainPtr->setPosition(from);
}

void GRFSerie::Arrow::setColor( const osg::Vec4& color )
{
	coneShape->setColor(color);
	boxShape->setColor(color);
}

const osg::Vec4& GRFSerie::Arrow::getColor() const
{
	return boxShape->getColor();
}
