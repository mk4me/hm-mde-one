#include "PCH.h"
#include "OsgSchemeDrawer.h"
#include "LineSchemeDrawer.h"

using namespace osg;
using namespace std;
using namespace kinematic;
using namespace boost;

void LineSchemeDrawer::draw()
{

}

void LineSchemeDrawer::update()
{
    auto connections = getVisualiztionScheme()->getConnections();
    auto joints = getVisualiztionScheme()->getStates();
        
    for (int i = connections.size() - 1; i >= 0; --i) {
        ref_ptr<Vec3Array> vertices = buffers[i];
        (*vertices)[0] = joints[connections[i].index1].position;
        (*vertices)[1] = joints[connections[i].index2].position;
        osg::Drawable* d = lines[i]->getDrawable(0);
        osg::Geometry* g = dynamic_cast<osg::Geometry*>(d);
        g->setVertexArray(vertices);
    }
}

void LineSchemeDrawer::deinit()
{

}

void LineSchemeDrawer::init( VisualizationSchemeConstPtr scheme )
{
    UTILS_ASSERT(scheme);
    OsgSchemeDrawer::init(scheme);
    node = new osg::Group;
    auto connections = getVisualiztionScheme()->getConnections();
    auto joints = getVisualiztionScheme()->getStates();

    buffers.reserve(connections.size());
    BOOST_FOREACH(const Connection& connection, connections) {
        addLine(joints[connection.index1].position, 
            joints[connection.index2].position, connection.color);
    }
}

osg::ref_ptr<osg::Node> LineSchemeDrawer::getNode()
{
    return node;
}

void LineSchemeDrawer::addLine( const osg::Vec3& from, const osg::Vec3& to, const osg::Vec4& color)
{
    GeodePtr geode = new Geode();
    lines.push_back(geode);
    osg::ref_ptr<Geometry> linesGeom = new osg::Geometry();

    ref_ptr<Vec3Array> vertices = new osg::Vec3Array(2);
    buffers.push_back(vertices);
    (*vertices)[0] = from;
    (*vertices)[1] = to;

    linesGeom->setVertexArray(vertices);

    ref_ptr<Vec4Array> colors = new osg::Vec4Array;
    colors->push_back(osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f));
    linesGeom->setColorArray(colors);
    linesGeom->setColorBinding(osg::Geometry::BIND_OVERALL);

    linesGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, 2));

    ref_ptr<StateSet> stateset = new osg::StateSet;
    stateset->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    geode->setStateSet(stateset);

    geode->addDrawable(linesGeom);
    node->addChild(geode);
}
