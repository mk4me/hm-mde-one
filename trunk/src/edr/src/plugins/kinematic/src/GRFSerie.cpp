#include "PCH.h"
#include "GRFSerie.h"

//using namespace core;
//using namespace osg;
//using namespace boost;
//using namespace std;

//! skala strzałki ukazującej siłę nacisku
const osg::Vec3 arrowScale(0.01f, 0.01f, 1.0f);
//! prog dla którego sensowne jest wizualizowanie odczytów GRF
const float treshold = 0.01f;

osg::ref_ptr<osg::Texture2D> GRFSerie::texture1;
osg::ref_ptr<osg::Texture2D> GRFSerie::texture2;

GRFSerie::GRFSerie( KinematicVisualizer * visualizer,
	const utils::TypeInfo & requestedType,
	const core::VariantConstPtr & data ) : visualizer(visualizer),
	maxLength(0.0f), data(data)
{
	UTILS_ASSERT(data->data()->getTypeInfo() == typeid(c3dlib::GRFCollection));
	this->requestedType = requestedType;
	//FIX tymczasowy dla linuxa
	grfCollection = data->get();	

	const c3dlib::IForcePlatformCollection& platforms = grfCollection->getPlatforms();
    if (!platforms.empty()) {
        matrixTransform->addChild(createPlatformsGroup(platforms));
        matrixTransform->addChild(createButterfly(grfCollection, this->maxLength));
    }

	for (auto it = platforms.begin(); it != platforms.end(); ++it) {
		auto range = (*it)->getSteps();
		for (auto step = range.begin(); step != range.end(); ++step) {
			ArrowPtr a1 = createArrow();
			matrixTransform->addChild(a1->mainPtr);
			a1->mainPtr->setNodeMask(0);
			GhostStackPtr ghost1(new GhostStack(8, matrixTransform, osg::Vec4(1,1,1,1)));
			stepsData[*step] = std::make_pair(a1, ghost1);
		}
	}
}

osg::Matrix GRFSerie::getInitialMatrix() const
{
	//osg::Matrix m;
	//m.setTrans(scheme->getRootPosition(0.0));
	return osg::Matrix();
}

osg::ref_ptr<osg::Group> GRFSerie::createPlatformsGroup(const c3dlib::IForcePlatformCollection& platforms)
{
	osg::ref_ptr<osg::Group> group = new osg::Group();
	int i = 0;

	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	stateset->setMode( GL_LIGHTING, osg::StateAttribute::ON );

	for ( auto it = platforms.cbegin(); it != platforms.cend(); ++it) {
		GeodePtr platformGeode = new osg::Geode();
		
		platformGeode->setStateSet(stateset);

		osg::ref_ptr<osg::Box> box = new osg::Box((*it)->getCenter() - osg::Vec3(0, 0, 0.025f), (*it)->getWidth(), (*it)->getLength(), 0.025f);
		
        osg::ref_ptr<osg::ShapeDrawable> platform1 = new osg::ShapeDrawable(box);
        platform2Shape[*it] = platform1;
		platform1->setColor(osg::Vec4(0.5f, 0.5f, 0.3f, 1.0f));
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

GRFSerie::GroupPtr GRFSerie::createButterfly(c3dlib::GRFCollectionConstPtr grf, float& maxLength) const
{
    osg::ref_ptr<osg::Group> group = new osg::Group();
	const c3dlib::IForcePlatformCollection& platforms = grf->getPlatforms();
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

GRFSerie::GeodePtr GRFSerie::createStep(c3dlib::IForcePlatform::IStepConstPtr step, float &maxLength, c3dlib::IForcePlatformConstPtr platform) const
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
	osg::Vec3 v;
    osg::Vec3 origin1 = platform->getCenter();

    osg::Vec4 highColor(1.0f, 0.0f, 0.0f, 0.7f);
    osg::Vec4 lowColor(0.0f, 1.0f, 0.0f, 0.0f);
    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;

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

void GRFSerie::setLocalTime( double time )
{
    try {
        const auto& platforms = grfCollection->getPlatforms();
        for (auto it = platforms.cbegin(); it != platforms.cend(); ++it) {
            auto f1 = (*it)->getForceChannel();
            osg::Vec3 v1(TimeAccessor::getValue(time, *f1));

            float ratio1 = v1.length() / maxLength;

            osg::Vec4 c1(ratio1, 1.0f - ratio1, 0.0f, 1.0f);

            v1 *= grfScale;
            v1[0] *= -(*it)->getSignX();
            v1[1] *= -(*it)->getSignY();
            v1[2] *= -1.0f;
       
            auto range = (*it)->getSteps();
            for (auto step = range.begin(); step != range.end(); ++step) {
                float startTime1 = (*step)->getStartTime();
                float endTime1	 = (*step)->getEndTime();

				osg::Vec3 start1 = (*step)->getStartPoint();
				osg::Vec3 end1   = (*step)->getEndPoint();

                float timeRatio1 = (time - startTime1) / (endTime1 - startTime1);
        
                osg::Vec3 origin1  = start1  * (1.0f - timeRatio1) + end1 * timeRatio1;
       
                // chowanie/pokazywanie strzalek, kolorowanie platform.
                auto data = stepsData.find(*step);
                if (data != stepsData.end()) {
                    auto a1 = data->second.first;
                    auto g1 = data->second.second;
                    auto platform1 = platform2Shape[*it];
                    if (timeRatio1 >= 0.0f && timeRatio1 <= 1.0f) {
                        a1->mainPtr->setNodeMask(1);
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

GRFSerie::ArrowPtr GRFSerie::createArrow()
{
	GeodePtr boxGeode = new osg::Geode();
	GeodePtr coneGeode = new osg::Geode();

	osg::ref_ptr<osg::Box> unitBox = new osg::Box(osg::Vec3(0, 0, 0), 0.5f, 0.5f, 0.9f);
	osg::ref_ptr<osg::Cone> unitCone = new osg::Cone(osg::Vec3(0, 0, 0), 1.0f, 0.1f);
	osg::ref_ptr<osg::ShapeDrawable> boxShape = new osg::ShapeDrawable(unitBox);
	osg::ref_ptr<osg::ShapeDrawable> coneShape = new osg::ShapeDrawable(unitCone);

	boxGeode->addDrawable(boxShape);
	coneGeode->addDrawable(coneShape);

	TransformPtr transformBox = new osg::PositionAttitudeTransform();
	TransformPtr transformCone = new osg::PositionAttitudeTransform();
	TransformPtr transform = new osg::PositionAttitudeTransform();

	transformBox->addChild(boxGeode);
	transformBox->setName("BOX");
	transformBox->setPosition(osg::Vec3(0, 0, 0.45f));
	transformCone->addChild(coneGeode);
	transformCone->setName("CONE");
	transformCone->setPosition(osg::Vec3(0, 0, 0.45f));

	transform->setScale(arrowScale);

	transform->addChild(transformBox);
	transformBox->addChild(transformCone);

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

const core::VariantConstPtr & GRFSerie::getData() const
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
	osg::Vec3 v;
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
	geometry->setColorBinding(osg::Geometry::BIND_OVERALL);
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
				throw loglib::runtime_error("Unable to load texture: images/1.png");
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
				throw loglib::runtime_error("Unable to load texture: images/2.png");
			}
			t->setImage(i);
			t->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::NEAREST);
			t->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::NEAREST);
			texture2 = t;
		}
		return texture2;
	} else {
		UTILS_ASSERT(false);
		throw loglib::runtime_error("Wrong texture number");
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

double GRFSerie::getBegin() const
{
	return 0.0;
}

double GRFSerie::getEnd() const
{
	return getLength();
}



void GRFSerie::Arrow::setArrow(osg::Vec3 from, osg::Vec3 to)
{
	osg::Vec3 dir = to - from;
	osg::Vec3 zero;
	osg::Vec3 up(0.0f, 1.0f, 0.0f);
	float length = dir.normalize();
    if (length < 0.01f) {
        dir.set(0,0,0.1f);
        length = 0.01f;
    }
	osg::Matrix mat;
	osg::Quat rotation;
	mat.makeLookAt(zero, -dir, up);
	rotation.set(osg::Matrix::inverse(mat));

	mainPtr->setScale(osg::Vec3(arrowScale[0], arrowScale[1], arrowScale[2] * length));
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
		a->setColor(osg::Vec4());
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
    static const float delta = color[3] / static_cast<float>(maxSize);

    for (auto it = stackArrows.begin(); it != stackArrows.end(); ++it) {
        ArrowPtr a = *it;
        const osg::Vec4& color = a->getColor();
        float alpha = color[3] - delta;
        if (alpha > 0) {
            a->mainPtr->setNodeMask(1);
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
