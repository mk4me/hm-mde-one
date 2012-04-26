#include "PCH.h"	
#include "GlLineSchemeDrawer.h"
using namespace osg;
using namespace boost;
using namespace std;

void GlLineSchemeDrawer::init( VisualizationSchemeConstPtr scheme )
{
    UTILS_ASSERT(scheme);
    node = new osg::Group;
    OsgSchemeDrawer::init(scheme);
    const auto& connections = getVisualiztionScheme()->getConnections();
    const auto& states = getVisualiztionScheme()->getStates();

    for (unsigned int i = 0;  i < connections.size(); ++i) {
        VisualizationScheme::State state1 = states[connections[i].index1];
        VisualizationScheme::State state2 = states[connections[i].index2];
        float length = connections[i].length > 0.0f ? connections[i].length : (state1.position - state2.position).length();
        TransformPtr t = addTransform(state1.position, state2.position, 
            useCustomColor ? customColor : connections[i].color,  length);
        node->addChild(t);
        cones.push_back(t);
    }
    update();
 }

void GlLineSchemeDrawer::deinit()
{
    // TODO -> unparent
}

void GlLineSchemeDrawer::update()
{
    const auto& connections = getVisualiztionScheme()->getConnections();
    const auto& states = getVisualiztionScheme()->getStates();
    float epsilon = 0.1f;
    for (int i = connections.size() - 1;  i >= 0; --i) {
        const VisualizationScheme::State& state1 = states[connections[i].index1];
        const VisualizationScheme::State& state2 = states[connections[i].index2];

        Vec3 from = state1.position;
        Vec3 to = state2.position;
        TransformPtr t = cones[i];
        Vec3 dir = to - from;
        Vec3 zero;
        Vec3 up(0.0f, 1.0f, 0.0f);
        //float length = dir.normalize();
        osg::Matrix mat;
		osg::Quat rotation;
		mat.makeLookAt(zero, -dir, up);
		rotation.set(Matrix::inverse(mat));
        t->setPosition(from);
        t->setAttitude(rotation);

        float length = connections[i].length;
        bool pointsVisible = state1.visible && state2.visible;
        bool correctLength = length < 0.0f || std::abs(length - dir.length()) < epsilon;
        t->setNodeMask(connections[i].visible && pointsVisible && correctLength ? 0xFFFF : 0);
    }
}

void GlLineSchemeDrawer::draw()
{

}

GlLineSchemeDrawer::TransformPtr GlLineSchemeDrawer::addTransform(const osg::Vec3& from, const osg::Vec3& to, const osg::Vec4& color, float length)
{
    //float rand = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
    
    GeodePtr geode = new Geode();
    Vec3 dir = to - from;
    Vec3 zero;
    Vec3 up(0.0f, 1.0f, 0.0f);
    dir.normalize();
    GeometryPtr cylinder = createCustomCylinder(complex, length, radius, color);
    
    osg::Matrix mat;
    mat.makeLookAt(zero, -dir, up);

    osg::Quat rotation;
    rotation.set(Matrix::inverse(mat));
    geode->addDrawable(cylinder);
    osg::ref_ptr<osg::StateSet> set = new osg::StateSet();
    set->setMode(GL_BLEND,osg::StateAttribute::ON);
    geode->setStateSet(set);

    TransformPtr transform = new PositionAttitudeTransform();
    transform->addChild(geode);
    transform->setPosition(from);
    transform->setAttitude(rotation);
    return transform;
}

GlLineSchemeDrawer::GeometryPtr GlLineSchemeDrawer::createCustomCylinder( int complex , float height, float radius, osg::Vec4 color) const
{
    osg::ref_ptr<osg::Geometry> cylinderGeom = new osg::Geometry();
    osg::ref_ptr<osg::Vec3Array> points = new osg::Vec3Array();
    osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array();

    int n = complex;
    double r = radius;
    double segmentRad = 2 * osg::PI / complex;
    //int numberOfSeparators = 4 * n + 4;

    //triangleIndices = new Int32Collection();
    osg::Vec3 v;
    //float height2 = 0.5f * height;
    
    for (int i = 0; i < complex; i++) {
        float rad = static_cast<float>(i) * segmentRad;
        float x = r * cosf(rad);
        float z = r * sinf(rad);

        v.set(x, z, height);
        points->push_back(v);
        v.set(x, z, 0);
        points->push_back(v);

        v.set(x, z, 0.0f);
        v.normalize();
        normals->push_back(v);
        normals->push_back(v);
    }
    v.set(0, 0, height);
    points->push_back(v);
    normals->push_back(v);
    v.set(0, 0, 0);
    points->push_back(v);   
    normals->push_back(v);
    
    cylinderGeom->setVertexArray(points);
    cylinderGeom->setNormalArray(normals);
    cylinderGeom->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
    cylinderGeom->setDataVariance(osg::Object::DYNAMIC);
    
    int mod = complex * 2;
    for (int i = 0; i < complex; i++) {
        osg::ref_ptr<osg::DrawElementsUInt> d = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
        d->push_back((i * 2    ) % mod);
        d->push_back((i * 2 + 1) % mod);
        d->push_back((i * 2 + 2) % mod);
        cylinderGeom->addPrimitiveSet(d);

        osg::ref_ptr<osg::DrawElementsUInt> d2 = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
        d2->push_back((i * 2 + 1) % mod);
        d2->push_back((i * 2 + 2) % mod);
        d2->push_back((i * 2 + 3) % mod);
        cylinderGeom->addPrimitiveSet(d2);
    }

    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
    colors->push_back(color);
    cylinderGeom->setColorArray(colors);
    cylinderGeom->setColorBinding(osg::Geometry::BIND_OVERALL);

    ref_ptr<StateSet> stateset = new osg::StateSet;
    return cylinderGeom;
}

