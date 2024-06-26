#include "PCH.h"
			
#include "GlPointSchemeDrawer.h"
using namespace osg;
using namespace boost;
using namespace std;

GlPointSchemeDrawer::GlPointSchemeDrawer( int sphereComplex, float sphereRadius ) :
    complex(sphereComplex),
    radius(sphereRadius),
    useCustomColor(false)
{
}

GlPointSchemeDrawer::GlPointSchemeDrawer(int sphereComplex, float sphereRadius, const osg::Vec4& color ) :
    complex(sphereComplex),
    radius(sphereRadius),
	customColor(color),
    useCustomColor(true)
{

}

void GlPointSchemeDrawer::init( VisualizationSchemeConstPtr scheme )
{
    UTILS_ASSERT(scheme);
    OsgSchemeDrawer::init(scheme);

    node = new osg::Group;
    auto markers = scheme->getStates();
    createMarkersCrowd(markers);
}

void GlPointSchemeDrawer::deinit()
{
    // TODO -> unparent
}

void GlPointSchemeDrawer::update()
{
    auto markers = getVisualiztionScheme()->getStates();
    for (int i = markers.size() - 1; i >= 0; --i) {
        points[i]->setNodeMask(markers[i].visible ? 0xFFFF : 0);
        points[i]->setPosition(markers[i].position);
        changeColorIfNecessary(points[i], markers[i]);
    }
}

void GlPointSchemeDrawer::draw()
{

}

void GlPointSchemeDrawer::createMarkersCrowd(const std::vector<MarkerState>& markers)
{
    int count = markers.size();
    points.resize(count);
    for (int i = 0; i < count; ++i) {
        points[i] = addPoint(markers[i].position, useCustomColor ? customColor : markers[i].color, markers[i].name);
        node->addChild(points[i]);
    }
}

GlPointSchemeDrawer::TransformPtr GlPointSchemeDrawer::addPoint( const osg::Vec3& point, const osg::Vec4& color, const std::string& name)
{
    GeodePtr marker = createMarker(color, 0.08f);
    marker->setName(name.c_str());
    ref_ptr<StateSet> stateset = new osg::StateSet;
    stateset->setMode( GL_LIGHTING, osg::StateAttribute::ON );
    stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
    marker->setStateSet(stateset);
    TransformPtr transform = new PositionAttitudeTransform();
    transform->addChild(marker);
    transform->setPosition(point);
    return transform;
}

GlPointSchemeDrawer::GeodePtr GlPointSchemeDrawer::createMarker(osg::Vec4 color, float scale)
{
    GlPointSchemeDrawer::GeodePtr geode = new Geode();

    std::map<osg::Vec4, GeometryPtr>::iterator found = spheresByColor.find(color);
    GeometryPtr customSphere;
    if (found != spheresByColor.end()) {
        customSphere = found->second;
    } else {
        customSphere = createCustomSphere(complex, color);
        spheresByColor[color] = customSphere;
    }
    geode->addDrawable(customSphere);
    return geode;
}

GlPointSchemeDrawer::GeometryPtr GlPointSchemeDrawer::createCustomSphere( int complex , osg::Vec4 color) const
{
    osg::ref_ptr<osg::Geometry> sphereGeom = new osg::Geometry();
    osg::ref_ptr<osg::Vec3Array> points = new osg::Vec3Array();
    osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array();

    int e;
    int n = complex;
    double r = radius;
    double segmentRad = osg::PI / 2.0 / (n + 1);
    int numberOfSeparators = 4 * n + 4;

    osg::Vec3 v;
    
    for (e = -n; e <= n; ++e) {
        double r_e = r * cosf(segmentRad * e);
        double y_e = r * sinf(segmentRad * e);

        for (int s = 0; s <= (numberOfSeparators - 1); ++s) {
            double z_s = r_e * sinf(segmentRad * s) * (-1);
            double x_s = r_e * cosf(segmentRad * s);
            v.set(x_s, y_e, z_s);
            points->push_back(v);
            v.normalize();
            normals->push_back(v);
        }
    }
    v.set(0, r, 0);
    points->push_back(v);
    v.normalize();
    normals->push_back(v);

    v.set(0, -1 * r, 0);
    points->push_back(v);
    v.normalize();
    normals->push_back(v);

    sphereGeom->setVertexArray(points);
    sphereGeom->setNormalArray(normals);
    sphereGeom->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
    sphereGeom->setDataVariance(osg::Object::DYNAMIC);
    

    for (e = 0; e < 2 * n; ++e) {
        for (int i = 0; i < numberOfSeparators; ++i) {
            osg::ref_ptr<osg::DrawElementsUInt> d = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
            d->push_back(e * numberOfSeparators + i);
            d->push_back(e * numberOfSeparators + i + numberOfSeparators);
            d->push_back(e * numberOfSeparators + (i + 1) % numberOfSeparators + numberOfSeparators);
            sphereGeom->addPrimitiveSet(d);

            osg::ref_ptr<osg::DrawElementsUInt> d2 = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
            d2->push_back(e * numberOfSeparators + (i + 1) % numberOfSeparators + numberOfSeparators);
            d2->push_back(e * numberOfSeparators + (i + 1) % numberOfSeparators);
            d2->push_back(e * numberOfSeparators + i);
            sphereGeom->addPrimitiveSet(d2);
        }
    }

    for (int i = 0; i < numberOfSeparators; ++i) {
        osg::ref_ptr<osg::DrawElementsUInt> d = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
        d->push_back(e * numberOfSeparators + i);
        d->push_back(e * numberOfSeparators + (i + 1) % numberOfSeparators);
        d->push_back(numberOfSeparators * (2 * n + 1));
        sphereGeom->addPrimitiveSet(d);
    }

    for (int i = 0; i < numberOfSeparators; ++i) {
        osg::ref_ptr<osg::DrawElementsUInt> d = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
        d->push_back(i);
        d->push_back((i + 1) % numberOfSeparators);
        d->push_back(numberOfSeparators * (2 * n + 1) + 1);
        sphereGeom->addPrimitiveSet(d);
    }

    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
    colors->push_back(color);
    sphereGeom->setColorArray(colors);
    sphereGeom->setColorBinding(osg::Geometry::BIND_OVERALL);

    ref_ptr<StateSet> stateset = new osg::StateSet;
    return sphereGeom;
}

void GlPointSchemeDrawer::changeColorIfNecessary( TransformPtr point, const VisualizationScheme::State& state )
{
    for (int i = point->getNumChildren() - 1; i >= 0; --i) {
        // punkt powinien zawierac geode ze sfera
        GeodePtr geode = point->getChild(i)->asGeode();
        if (geode && geode->getName() == state.name) {
            UTILS_ASSERT(geode->getNumDrawables() == 1);
            // sprawdzamy, czy istnieje sfera w odpowiednim kolorze
            std::map<osg::Vec4, GeometryPtr>::iterator found = spheresByColor.find(state.color);
            GeometryPtr customSphere;
            if (found != spheresByColor.end()) {
                if (found->second == geode->getDrawable(0)) {
                    // sfera istnieje, i jest ta sama, która jest podpięta pod geode
                    // oznacza to, ze kolor jest ten sam, nic nie trzeba robic
                    return;
                } else {
                    // kolor się zmienił, ale mamy już sfere w odpowiednim kolorze,
                    // podpinamy ja pod geode
                    geode->removeDrawable(geode->getDrawable(0));
                    geode->addDrawable(found->second);
                }
            } else {
                // nie ma sfery w odpowiednim kolorze, trzeba ja sobie stworzyć.
                // complex - dokładność sfery
                auto newSphere = createCustomSphere(complex, state.color);
                spheresByColor[state.color] = newSphere;
                // stara sfere trzeba usunąć z node'a
                geode->removeDrawable(geode->getDrawable(0));
                geode->addDrawable(newSphere);
            }
            break;
        }
    }
}




