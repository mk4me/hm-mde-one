#include "PCH.h"
#include "GRFSerie.h"

using namespace osg;
using namespace boost;
using namespace std;

//! skala strzałki ukazującej siłę nacisku
const osg::Vec3 arrowScale(0.01f, 0.01f, 1.0f);
//! prog dla którego sensowne jest wizualizowanie odczytów GRF
const float treshold = 0.01f;

osg::ref_ptr<osg::Texture2D> GRFSerie::texture1;
osg::ref_ptr<osg::Texture2D> GRFSerie::texture2;

GRFSerie::GRFSerie( KinematicVisualizer * visualizer ) : 
visualizer(visualizer),
    maxLength(0.0f)
{

}

osg::ref_ptr<osg::Group> GRFSerie::createPlatformsGroup( const IForcePlatformCollection& platforms)
{
	osg::ref_ptr<osg::Group> group = new osg::Group();
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(0.5f, 0.5f, 0.3f, 1.0f));
	osg::Vec3 up(0, 0, -0.00001f);
	int i = 0;
	for ( auto it = platforms.cbegin(); it != platforms.cend(); ++it) {
		GeodePtr platformGeode = new osg::Geode();

		//GeometryPtr platformLines = new osg::Geometry();
		osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
		stateset->setMode( GL_LIGHTING, osg::StateAttribute::ON );
		platformGeode->setStateSet(stateset);

        osg::ref_ptr<osg::Box> box = new osg::Box((*it)->getCenter() - Vec3(0,0, 0.025f), (*it)->getWidth(), (*it)->getLength(), 0.025f);		
		
        osg::ref_ptr<osg::ShapeDrawable> platform1 = new osg::ShapeDrawable(box);
        platform2Shape[*it] = platform1;
        platform1->setColor(Vec4(0.5f, 0.5f, 0.3f, 1.0f));
        platformGeode->addDrawable(platform1.get());
		
		++i;

        osg::ref_ptr<osg::Texture2D> tex;
        
		TransformPtr pform;
        if (tryGetTexture(tex, i)) {
            pform = createPlatformTransform(tex, (*it)->getCenter(), (*it)->getWidth() * 0.7f, (*it)->getLength() * 0.7f, 0.025f);
            group->addChild(pform);
        }		
		group->addChild(platformGeode);
	}

	return group;
}

GRFSerie::GroupPtr GRFSerie::createButterfly( GRFCollectionConstPtr grf, float& maxLength) const
{
    osg::ref_ptr<osg::Group> group = new osg::Group();
    const IForcePlatformCollection& platforms = grf->getPlatforms();
    for (auto it = platforms.begin(); it != platforms.end(); ++it) {
        auto range = (*it)->getSteps();
        for (auto step = range.begin(); step != range.end(); ++step) {
            if ((*step)->isValid()) {
                group->addChild(createStep(*step, maxLength, *it));
            }
        }
    }
    
    return group;
}

//GRFSerie::GeodePtr GRFSerie::createPlatformButterfly(IForcePlatformConstPtr platform, GRFCollectionConstPtr grf, float& maxLength) const
//{
//    IForcePlatform::IStepConstPtr step = *platform->getSteps().begin();
//    return createStep(step, grf, maxLength, platform);
//
//}

GRFSerie::GeodePtr GRFSerie::createStep( IForcePlatform::IStepConstPtr step, float &maxLength, IForcePlatformConstPtr platform) const
{
    osg::Vec3 start1 = step->getStartPoint();
    osg::Vec3 end1   = step->getEndPoint();
    float startTime1 = step->getStartTime();
    float endTime1	 = step->getEndTime();

    int numSegments = 300;
    auto f1 = platform->getForceChannel();
    float delta = (f1->getLength() / static_cast<float>(numSegments));
    GeodePtr geode = new osg::Geode();
    osg::ref_ptr<osg::Vec3Array> verts = new osg::Vec3Array;
    Vec3 v;
    osg::Vec3 origin1 = platform->getCenter();

    osg::Vec4 highColor(1.0f, 0.0f, 0.0f, 0.7f);
    osg::Vec4 lowColor(0.0f, 1.0f, 0.0f, 0.0f);
    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
    //colors->push_back(osg::Vec4(1.0f, 0.3f, 0.3f, 1.0f));

    maxLength = 0.0f;

    float f = 0.0f;
    float lengthBuffer = f1->getLength();
    osg::Vec3 lastV1;
    osg::Vec3 lastOrigin1;
    for (int i = 0; i < numSegments; ++i) {
        v.set(TimeAccessor::getValue(f, *f1));
        float length = v.length();

        float ratio1 = (f - startTime1) / (endTime1 - startTime1);
        origin1  = start1  * (1.0f - ratio1) + end1 * ratio1;
        v *= grfScale;
        v[0] *= -platform->getSignX();
        v[1] *= -platform->getSignY();
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

        if (length > maxLength) {
            maxLength = length;
        }

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
    lines->setDataVariance(osg::Object::DYNAMIC);
    geode->addDrawable(lines);
    return geode;
}

/*
GRFSerie::GroupPtr GRFSerie::createButterfly( GRFCollectionConstPtr grf, float& maxLength ) const
{
	osg::ref_ptr<osg::Group> group = new osg::Group();
    if (f1->hasStartEndData() && f2->hasStartEndData()) {
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
            v.set(getChannelValue(f, *f1));
            float length = v.length();

            float ratio1 = (f - startTime1) / (endTime1 - startTime1);
            float ratio2 = (f - startTime2) / (endTime2 - startTime2);
            origin1  = start1  * (1.0f - ratio1) + end1 * ratio1;
            origin2 = start2  * (1.0f - ratio2) + end2 * ratio2;
            v *= grfScale;
            v[0] *= grf->getPlatforms()[0]->getSignX();
            v[1] *= grf->getPlatforms()[0]->getSignY();
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
            v.set(getChannelValue(f, *f2));
            length = v.length();
            v *= grfScale;
            v[0] *= grf->getPlatforms()[1]->getSignX();
            v[1] *= grf->getPlatforms()[1]->getSignY();
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
    }
	return group;
}
*/
void GRFSerie::setLocalTime( double time )
{
    float t = time;
    try {
        const auto& platforms = grfCollection->getPlatforms();
        for (auto it = platforms.cbegin(); it != platforms.cend(); ++it) {
            auto f1 = (*it)->getForceChannel();
            osg::Vec3 v1(TimeAccessor::getValue(t, *f1));

            float ratio1 = v1.length() / maxLength;

            osg::Vec4 c1(ratio1, 1.0f - ratio1, 0.0f, 1.0f);

            v1 *= grfScale;
            v1[0] *= -(*it)->getSignX();
            v1[1] *= -(*it)->getSignY();
            v1[2] *= -1.0f;
       
            auto range = (*it)->getSteps();
            for (auto step = range.begin(); step != range.end(); ++step) {
                osg::Vec3 start1 = (*step)->getStartPoint();
                osg::Vec3 end1   = (*step)->getEndPoint();
                float startTime1 = (*step)->getStartTime();
                float endTime1	 = (*step)->getEndTime();

                float timeRatio1 = (time - startTime1) / (endTime1 - startTime1);
        
                osg::Vec3 origin1  = start1  * (1.0f - timeRatio1) + end1 * timeRatio1;
       
                // chowanie/pokazywanie strzalek, kolorowanie platform.
                auto data = stepsData.find(*step);
                if (data != stepsData.end()) {
                    auto a1 = data->second.first;
                    auto g1 = data->second.second;
                    auto platform1 = platform2Shape[*it];
                    if (timeRatio1 >= 0.0f && timeRatio1 <= 1.0f) {
                        a1->mainPtr->setNodeMask(0xffff);
                        a1->setArrow(origin1, v1 + origin1);	
                        a1->setColor(c1);
                        g1->addState(std::make_pair(origin1, v1 + origin1));
                        platform1->setColor(osg::Vec4(0.4f, 0.4f, 0.0f, 1.0f));
                    } else {
                        a1->mainPtr->setNodeMask(0);
                        platform1->setColor(osg::Vec4(0.5f, 0.5f, 0.3f, 1.0f));
                    }

                    g1->update();
                }
            }
            
        }
    } catch(...) {
    }
}

//void GRFSerie::setLocalTime( double time )
//{
//	try {
//		float t = time;
//        auto f1 = grfCollection->getGRFChannel(GRFChannel::F1);
//        auto f2 = grfCollection->getGRFChannel(GRFChannel::F2);
//		osg::Vec3 v1(getChannelValue(t, *f1));
//		osg::Vec3 v2(getChannelValue(t, *f2));
//
//		float ratio1 = v1.length() / maxLength;
//		float ratio2 = v2.length() / maxLength;
//
//		osg::Vec4 c1(ratio1, 1.0f - ratio1, 0.0f, 1.0f);
//		osg::Vec4 c2(ratio2, 1.0f - ratio2, 0.0f, 1.0f);
//
//		v1 *= grfScale;
//        v1[0] *= grfCollection->getPlatforms()[0]->getSignX();
//        v1[1] *= grfCollection->getPlatforms()[0]->getSignY();
//		v1[2] *= -1.0f;
//		v2 *= grfScale;
//        v2[0] *= grfCollection->getPlatforms()[1]->getSignX();
//        v2[1] *= grfCollection->getPlatforms()[1]->getSignY();
//		v2[2] *= -1.0f;
//		//osg::Vec3 origin1 = grfCollection->getPlatforms()[0]->getCenter();
//		//osg::Vec3 origin2 = grfCollection->getPlatforms()[1]->getCenter();
//
//		osg::Vec3 start1 = f1->getStartPoint();
//		osg::Vec3 end1   = f1->getEndPoint();
//		osg::Vec3 start2 = f2->getStartPoint();
//		osg::Vec3 end2   = f2->getEndPoint();
//		float startTime1 = f1->getDataStartTime();
//		float endTime1	 = f1->getDataEndTime();
//		float startTime2 = f2->getDataStartTime();
//		float endTime2	 = f2->getDataEndTime();
//
//		float timeRatio1 = (time - startTime1) / (endTime1 - startTime1);
//		float timeRatio2 = (time - startTime2) / (endTime2 - startTime2);
//
//		osg::Vec3 origin1  = start1  * (1.0f - timeRatio1) + end1 * timeRatio1;
//		osg::Vec3 origin2 = start2  * (1.0f - timeRatio2) + end2 * timeRatio2;
//		
//		// chowanie/pokazywanie strzalek, kolorowanie platform.
//		//if (v1.length2() > treshold) {
//		if (timeRatio1 >= 0.0f && timeRatio1 <= 1.0f) {
//			a1->mainPtr->setNodeMask(0xffff);
//			a1->setArrow(origin1, v1 + origin1);	
//			a1->setColor(c1);
//			g1->addState(std::make_pair(origin1, v1 + origin1));
//			platform1->setColor(osg::Vec4(0.4f, 0.4f, 0.0f, 1.0f));
//		} else {
//			a1->mainPtr->setNodeMask(0);
//			platform1->setColor(osg::Vec4(0.5f, 0.5f, 0.3f, 1.0f));
//		}
//
//		//if (v2.length2() > treshold) {
//		if (timeRatio2 >= 0.0f && timeRatio2 <= 1.0f) {
//			a2->setColor(c2);
//			a2->setArrow(origin2, v2 + origin2);
//			a2->mainPtr->setNodeMask(0xffff);
//			g2->addState(std::make_pair(origin2, v2 + origin2));
//			platform2->setColor(osg::Vec4(0.4f, 0.4f, 0.0f, 1.0f));
//		} else {
//			a2->mainPtr->setNodeMask(0);
//			platform2->setColor(osg::Vec4(0.5f, 0.5f, 0.3f, 1.0f));
//		}
//
//		g1->update();
//		g2->update();
//	} catch(...) {
//	}
//}

GRFSerie::ArrowPtr GRFSerie::createArrow()
{
	GeodePtr boxGeode = new osg::Geode();
	GeodePtr coneGeode = new osg::Geode();

	osg::ref_ptr<osg::Box> unitBox = new osg::Box(Vec3(0, 0, 0), 0.5f, 0.5f, 0.9f);
	osg::ref_ptr<osg::Cone> unitCone = new osg::Cone(Vec3(0, 0, 0), 1.0f, 0.1f);
	osg::ref_ptr<osg::ShapeDrawable> boxShape = new osg::ShapeDrawable(unitBox);
	osg::ref_ptr<osg::ShapeDrawable> coneShape = new osg::ShapeDrawable(unitCone);

	boxGeode->addDrawable(boxShape);
	coneGeode->addDrawable(coneShape);

	TransformPtr transformBox = new osg::PositionAttitudeTransform();
	TransformPtr transformCone = new osg::PositionAttitudeTransform();
	TransformPtr transform = new osg::PositionAttitudeTransform();

	transformBox->addChild(boxGeode);
	transformBox->setName("BOX");
	transformBox->setPosition(Vec3(0, 0, 0.45f));
	transformCone->addChild(coneGeode);
	transformCone->setName("CONE");
	transformCone->setPosition(Vec3(0, 0, 0.45f));

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

void GRFSerie::setData(const utils::TypeInfo & requestedType, const core::ObjectWrapperConstPtr & data )
{
	UTILS_ASSERT(data->getTypeInfo() == typeid(GRFCollection));
    this->data = data;
	this->requestedType = requestedType;
	grfCollection = data->clone()->get();
    
    const IForcePlatformCollection& platforms = grfCollection->getPlatforms();
    transformNode->addChild(createPlatformsGroup(platforms));
	transformNode->addChild(createButterfly(grfCollection, this->maxLength));
    
    for (auto it = platforms.begin(); it != platforms.end(); ++it) {
        auto range = (*it)->getSteps();
        for (auto step = range.begin(); step != range.end(); ++step) {
            ArrowPtr a1 = createArrow();
            a1->mainPtr->setNodeMask(0);
            transformNode->addChild(a1->mainPtr);
            GhostStackPtr ghost1(new GhostStack(8, transformNode, osg::Vec4(1,1,1,1)));
            stepsData[*step] = std::make_pair(a1, ghost1);
        }
    }
}

const core::ObjectWrapperConstPtr & GRFSerie::getData() const
{
    return data;
}
GRFSerie::TransformPtr GRFSerie::createPlatformTransform(osg::Texture2D* texture, const osg::Vec3& origin, float width, float length, float height) const
{
	float width2 = 0.5f * width;
	float height2 = 0.5001f * height;
	float lenght2 = 0.5f * length;

	TransformPtr transform = new osg::PositionAttitudeTransform();
	
	GeodePtr geode = new osg::Geode();
	//transform->addChild(geode);
	GeometryPtr geometry = new osg::Geometry();
	//geode->addDrawable(geometry);
	osg::ref_ptr<osg::Vec3Array> verts = new osg::Vec3Array;
	Vec3 v;
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	osg::ref_ptr<osg::Vec2Array> coord = new osg::Vec2Array;

	colors->push_back(osg::Vec4(1,1,1,1));

	verts->push_back(osg::Vec3(-width2, -lenght2, height2));
	
	coord->push_back(osg::Vec2(0.0f, 0.0f));
	verts->push_back(osg::Vec3(width2, -lenght2, height2));
	coord->push_back(osg::Vec2(1.0f, 0.0f));
	verts->push_back(osg::Vec3(width2, lenght2, height2));
	coord->push_back(osg::Vec2(1.0f, 1.0f));
	verts->push_back(osg::Vec3(-width2, lenght2, height2));
	coord->push_back(osg::Vec2(0.0f, 1.0f));
	
	geometry->setVertexArray(verts);
    geometry->setDataVariance(osg::Object::DYNAMIC);
	geometry->setTexCoordArray(0, coord);
	
	geometry->setColorArray(colors);
	geometry->setColorBinding(Geometry::BIND_OVERALL);
	geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, verts->size()));
	//geometry->setPrimitiveSet(0, indices);
	
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	stateset->setMode( GL_LIGHTING, osg::StateAttribute::ON );
	stateset->setMode( GL_BLEND, osg::StateAttribute::ON );
	stateset->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
    stateset->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
	geode->setStateSet(stateset);

	geode->addDrawable(geometry);
	transform->setPosition(origin);
	transform->addChild(geode);	

	return transform;
}

osg::ref_ptr<osg::Texture2D> GRFSerie::getTexture( int number )
{
	if (number == 1) {
		if (!texture1) {
			osg::Texture2D* t = new osg::Texture2D;
			t->setDataVariance(osg::Object::DYNAMIC); 
			osg::Image* i = osgDB::readImageFile(plugin::getResourcePath("images/1.png").string());
			if (!i) {
				throw std::runtime_error("Unable to load texture: images/1.png");
			}
			t->setImage(i);
			t->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::NEAREST);
			t->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::NEAREST);
			texture1 = t;
		}
		return texture1;
	} else if (number == 2) {
		if (!texture2) {
			osg::Texture2D* t = new osg::Texture2D;
			t->setDataVariance(osg::Object::DYNAMIC); 
			osg::Image* i = osgDB::readImageFile(plugin::getResourcePath("images/2.png").string());
			if (!i) {
				throw std::runtime_error("Unable to load texture: images/2.png");
			}
			t->setImage(i);
			t->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::NEAREST);
			t->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::NEAREST);
			texture2 = t;
		}
		return texture2;
	} else {
		UTILS_ASSERT(false);
		throw std::runtime_error("Wrong texture number");
	}
}

bool GRFSerie::tryGetTexture( osg::ref_ptr<osg::Texture2D>& ret, int number )
{
    try {
        ret = getTexture(number);
    }
    catch (...) {
        return false;
    }
    return true;
}

void GRFSerie::setName( const std::string & name )
{
    this->name = name;
}

const std::string GRFSerie::getName() const
{
    return name;
}

double GRFSerie::getLength() const
{
    UTILS_ASSERT(grfCollection);
    return grfCollection->getLength();
}



void GRFSerie::Arrow::setArrow( Vec3 from, Vec3 to )
{
	Vec3 dir = to - from;
	Vec3 zero;
	Vec3 up(0.0f, 1.0f, 0.0f);
	float length = dir.normalize();
    if (length < 0.01f) {
        dir.set(0,0,0.1f);
        length = 0.01f;
    }
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



GRFSerie::GhostStack::GhostStack( int maxSize, GroupPtr hookNode, const osg::Vec4& color ) :
maxSize(maxSize) ,
    hookNode(hookNode),
    color(color)
{
    UTILS_ASSERT(maxSize > 0);
    for (int i = 0; i < maxSize; ++i) {
        ArrowPtr a = createArrow();
        hookNode->addChild(a->mainPtr);
        a->mainPtr->setNodeMask(0);
        stackArrows.push_back(a);
    }
}

void GRFSerie::GhostStack::addState( const ArrowState& state )
{
    int no = stackArrows.size();
    if (no < maxSize - 1) {
        ArrowPtr a = createArrow();
        a->setArrow(state.first, state.second);
        a->setColor(color);
        hookNode->addChild(a->mainPtr);
        stackArrows.push_back(a);
    } else {
        ArrowPtr a = *stackArrows.begin();
        stackArrows.pop_front();
        stackArrows.push_back(a);
        a->setArrow(state.first, state.second);
        a->setColor(color);
    }
}

void GRFSerie::GhostStack::update()
{
    float delta = color[3] / static_cast<float>(maxSize);

    for (auto it = stackArrows.begin(); it != stackArrows.end(); ++it) {
        ArrowPtr a = *it;
        const osg::Vec4& color = a->getColor();
        float alpha = color[3] - delta;
        if (alpha > 0) {
            a->mainPtr->setNodeMask(0xffff);
            a->setColor(osg::Vec4(color[0], color[1], color[2], alpha));
        } else {
            a->mainPtr->setNodeMask(0);
        }
    }
}

void GRFSerie::update()
{

}

const utils::TypeInfo & GRFSerie::getRequestedDataType() const
{
	return requestedType;
}
