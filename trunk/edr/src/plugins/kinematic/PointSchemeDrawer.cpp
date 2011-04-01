#include "PCH.h"
#include "PointSchemeDrawer.h"

using namespace osg;
using namespace boost;
using namespace std;


void PointSchemeDrawer::init( SkeletalVisualizationSchemeConstPtr scheme )
{
    UTILS_ASSERT(scheme);
    OsgSchemeDrawer::init(scheme);

    node = new osg::Group;
    auto markers = scheme->getMarkersStates();
    createMarkersCrowd(markers);
}

void PointSchemeDrawer::deinit()
{
    // TODO -> unparent
}

void PointSchemeDrawer::update()
{
    auto markers = getVisualiztionScheme()->getMarkersStates();
    for (int i = markers.size() - 1; i >= 0; --i) {
        points[i]->setPosition(markers[i].position);
    }
}

void PointSchemeDrawer::draw()
{

}

void PointSchemeDrawer::createMarkersCrowd(const std::vector<MarkerState>& markers)
{
    int count = markers.size();
    points.resize(count);
    for (int i = 0; i < count; ++i) {
        Vec3 position = markers[i].position;
        points[i] = addPoint(position);
        node->addChild(points[i]);
    }
}

PointSchemeDrawer::GeodePtr PointSchemeDrawer::createMarker(const osg::Vec4& color, float scale) const
{
    PointSchemeDrawer::GeodePtr geode = new Geode();

    ref_ptr<Sphere> sphere = new Sphere(Vec3(),  scale);
    ref_ptr<ShapeDrawable> drawable = new ShapeDrawable(sphere);
    
    drawable->setColor(color);
    geode->addDrawable(drawable);
    return geode;
}

PointSchemeDrawer::TransformPtr PointSchemeDrawer::addPoint( const osg::Vec3& point )
{
    float rand = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
    Vec4 color(rand, 1, rand, 1);
    GeodePtr marker = createMarker(color, 0.08f);
    TransformPtr transform = new PositionAttitudeTransform();
    transform->addChild(marker);
    transform->setPosition(point);
    return transform;
}