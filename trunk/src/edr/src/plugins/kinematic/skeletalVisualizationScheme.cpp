#include "PCH.h"
#include <plugins/kinematic/skeletalVisualizationScheme.h>
#include "uniqueCollection.h"
#include "ISchemeDrawer.h"

using namespace std;
using namespace osg;
using namespace boost;
using namespace kinematic;

SkeletalVisualizationScheme::SkeletalVisualizationScheme() :
    normalizedTime(0)
{
}

void SkeletalVisualizationScheme::setNormalizedTime( double val )
{
    UTILS_ASSERT(kinematicModel);
    if (normalizedTime != val) {
        normalizedTime = val;
        if (val < 0.0 || val > 1.0) {
            LOGGER(Logger::Debug, "SkeletalVisualizationScheme : value out of <0,1>");
        }

        if (kinematicModel->getSkeleton()) {
            updateJointTransforms(val);
        }
        if (kinematicModel->getMarkers()) {
            updateMarkers(val);
        }
    }
}

osg::Quat rotationParentChild(JointPtr parent, JointPtr child)
{
    double mul  = osg::DegreesToRadians(1.0);
    osg::Matrix matParZ; matParZ.makeRotate(-parent->axis[2] * mul, 0.0, 0.0, 1.0);
    osg::Matrix matParY; matParY.makeRotate(-parent->axis[1] * mul, 0.0, 1.0, 0.0);
    osg::Matrix matParX; matParX.makeRotate(-parent->axis[0] * mul, 1.0, 0.0, 0.0);

    osg::Matrix matPar = matParZ * matParY * matParX;
    //osg::Quat rz; rz.makeRotate(-parent->axis[2] * mul, 0.0, 0.0, 1.0);      
   // osg::Quat ry; ry.makeRotate(-parent->axis[1] * mul, 0.0, 1.0, 0.0);  
   // osg::Quat rx; rx.makeRotate(-parent->axis[0] * mul, 1.0, 0.0, 0.0);

    //osg::Quat par = rz * ry * rx;
    //osg::Quat par = rz;


    osg::Matrix matChiZ; matChiZ.makeRotate(child->axis[2] * mul, 0.0, 0.0, 1.0);
    osg::Matrix matChiY; matChiY.makeRotate(child->axis[1] * mul, 0.0, 1.0, 0.0);
    osg::Matrix matChiX; matChiX.makeRotate(child->axis[0] * mul, 1.0, 0.0, 0.0);

    osg::Matrix matChi = matChiX * matChiY * matChiZ;
    osg::Matrix resM = matChi * matPar ;
    Quat res; res.set(resM);
    return res;
}

osg::Vec3 vectorRotation(Vec3 v, double a, double b , double c)
{
    Quat rz; rz.makeRotate(c, 0,0,1);
    Quat ry; ry.makeRotate(b, 0,1,0);
    Quat rx; rx.makeRotate(a, 1,0,0);

    v = rz * v;
    v = ry * v;
    v = rx * v;
    return v;
}

void SkeletalVisualizationScheme::updateJointTransforms(const std::vector<osg::Quat>& rotations, hAnimJointPtr joint, Quat parentRot, Vec3 parentPos)
{
    // zapewnienie zgodnosci indeksow (miedzy tablicami connections i jointStates)
    int i = visJoints[joint];
    Quat ident;
    double mul = osg::DegreesToRadians(1.0);
    Vec3 shift = joint->getLocalShift();
       
    Quat pc = joint->getChildParentRotation();
	int index = kinematicModel->getSkeleton()->tryGetIndex(joint->getName());
    //auto it = rotations.find(joint->name);
    Quat rotation = index >= 0 ? rotations[index] * pc  * parentRot : pc * parentRot;

    shift = rotation * shift;

    jointMarkersStates[i].position = (parentPos + shift);

	// rewizja - przechodzenie po hierarchii
	BOOST_FOREACH(hAnimJointPtr child, joint->getActiveJointChildren()) {
		updateJointTransforms(rotations, child, rotation, parentPos + shift);
	}
}

void SkeletalVisualizationScheme::updateJointTransforms( double normalizedTime )
{
    UTILS_ASSERT(kinematicModel->getSkeleton());
    counterHelper = 0;

	JointAnglesCollectionPtr joints = kinematicModel->getSkeleton();
    hAnimSkeletonPtr skeleton = joints->getHAnimSkeleton();
    osg::Quat q; Vec3 pos;
    pos = joints->getRootPosition(normalizedTime);
    updateJointTransforms(joints->getValues(static_cast<float>(normalizedTime * kinematicModel->getSkeleton()->getLength())), skeleton->getRoot(), q, pos);
 }

void SkeletalVisualizationScheme::updateWorldTransforms( osg::Vec3 worldPosition, osg::Quat worldRotation )
{
    throw kinematic::NotYetImplemented("");
}

void SkeletalVisualizationScheme::updateSkinnedTransforms()
{
    throw kinematic::NotYetImplemented("");
}

//void SkeletalVisualizationScheme::setSchemeDrawer( ISchemeDrawerPtr drawer )
//{
//   if (this->schemeDrawer) {
//       this->schemeDrawer->deinit();
//   }
//
//   this->schemeDrawer = drawer;
//   this->schemeDrawer->init(weak);
//}

core::shared_ptr<SkeletalVisualizationScheme> SkeletalVisualizationScheme::create()
{
    core::shared_ptr<SkeletalVisualizationScheme> scheme(new SkeletalVisualizationScheme());
    scheme->weak = scheme;
    return scheme;
}

void SkeletalVisualizationScheme::setKinematicModel( KinematicModelConstPtr val )
{
    kinematicModel = val;

    if (kinematicModel->getSkeleton() && kinematicModel->getSkeleton()->getSkeletalData()) {
        const auto& jointMap = kinematicModel->getSkeleton()->getHAnimSkeleton()->getJoints();
        int count = jointMap.size();
        //jointMarkersStates.reserve(count + 5);
        
        auto it = jointMap.begin();
        for (int index = 0; it != jointMap.end(); it++) {
            if (it->second->isActive()) {
                visJoints[it->second] = index++;
            }
        }

        jointMarkersStates.resize(visJoints.size());
        osg::Vec4 gold(1,1,0,1);
        for (unsigned int i = 0; i < visJoints.size(); i++) {
            jointMarkersStates[i].color = gold;
        }

        hAnimSkeletonPtr skeleton = kinematicModel->getSkeleton()->getHAnimSkeleton();

        createSkeletonConnections(skeleton->getRoot());
		//rewizja
        updateJointTransforms(0.0);
    }

    if (kinematicModel->getMarkers()) {
        MarkerCollectionConstPtr markers = kinematicModel->getMarkers();
        int count =  markers->getNumChannels();
        if (count && markersStates.size() != count) {
            markersStates.resize(count);
        }
        osg::Vec4 blue(0,0,1,1);
        for (int i = 0; i < count; i++) {
            markersStates[i].position = markers->getValue(i, 0.0 );
            markersStates[i].color = blue;
        }
    }
    
}


void SkeletalVisualizationScheme::createSkeletonConnections(kinematic::hAnimJointPtr joint)
{ 
	// rewizja - hierarchia
    Vec4 color(1, 1, 1, 0.5f);
	BOOST_FOREACH(hAnimJointPtr child, joint->getActiveJointChildren()) {
		Connection c;
		c.index1 = visJoints[joint];
		c.index2 = visJoints[child];
		c.color = color;
		jointConnections.push_back(c);
		createSkeletonConnections(child);
	}
	/*BOOST_FOREACH(hAnimBonePtr bone, joint->GetChildrenBones()) {
		BOOST_FOREACH(hAnimJointPtr child, bone->childrenJoints) {
			if (child->isActive()) {
				Connection c;
				c.index1 = visJoints[lastActive];
				c.index2 = visJoints[child];
				c.color = color;
				jointConnections.push_back(c);
				lastActive = child;
			}
			createSkeletonConnections(child, lastActive);
		}
	}*/
}

void SkeletalVisualizationScheme::updateMarkers( double normalizedTime )
{
    MarkerCollectionConstPtr markers = kinematicModel->getMarkers();
    if (markers) {
        int count =  markers->getNumChannels();
        for (int i = 0; i < count; i++) {
            markersStates[i].position = markers->getValue(i, normalizedTime * kinematicModel->getMarkers()->getLength());
        }
    }
}

void SkeletalVisualizationScheme::setMarkersDataFromVsk( kinematic::VskParserConstPtr vsk )
{
    UTILS_ASSERT(kinematicModel && kinematicModel->getMarkers(), "There are no markers in kinematic model");

    std::map<std::string, int> markersIndices;
    MarkerCollectionConstPtr markers = kinematicModel->getMarkers();
    for (int i = markers->getNumChannels() - 1; i >= 0; --i) {
        markersIndices[markers->getMarkerName(i)] = i;
    }
   
    auto vskMarkers = vsk->getMarkers();
    for (auto it = vskMarkers.first; it != vskMarkers.second; it++) {
        std::map<std::string, int>::iterator found = markersIndices.find(it->name);
        if (found != markersIndices.end()) {
            markersStates[found->second].color = it->color;
        }
    }
    auto sticks = vsk->getSticks();
    markerConnections.reserve(std::distance(sticks.first, sticks.second));
    for (auto it = sticks.first; it != sticks.second; it++) {
        Connection c;
        auto it1 = markersIndices.find(it->name1);
        auto it2 = markersIndices.find(it->name2);
        if (it1 != markersIndices.end() && it2 != markersIndices.end()) {
            c.index1 = markersIndices[it->name1];
            c.index2 = markersIndices[it->name2];
            c.color = it->color;
            markerConnections.push_back(c);
        }
    }
}

const std::vector<SkeletalVisualizationScheme::JointState> & SkeletalVisualizationScheme::getStates( DataToDraw toDraw ) const
{
    if (toDraw == DrawSkeleton) {
        return getJointStates();
    } else if (toDraw == DrawMarkers) {
        return getMarkersStates();
    }

    UTILS_ASSERT(false);
    return getJointStates();
}

const std::vector<SkeletalVisualizationScheme::Connection> & SkeletalVisualizationScheme::getConnections(DataToDraw toDraw ) const
{
    if (toDraw == DrawSkeleton) {
        return getJointConnections();
    } else if (toDraw == DrawMarkers) {
        return getMarkerConnections();
    }

    UTILS_ASSERT(false);
    return getJointConnections();
}

//void PointSchemeDrawer::init( SkeletalVisualizationSchemeWeak scheme )
//{
//    UTILS_ASSERT(scheme.lock());
//    OsgSchemeDrawer::init(scheme);
//
//    node = new osg::Group;
//    auto markers = scheme.lock()->getJointMarkersStates();
//    createMarkersCrowd(markers);
//}
//
//void PointSchemeDrawer::deinit()
//{
//    // TODO -> unparent
//}
//
//void PointSchemeDrawer::update( double time )
//{
//    auto markers = getVisualiztionScheme().lock()->getJointMarkersStates();
//    for (int i = markers.size() - 1; i >= 0; --i) {
//        points[i]->setPosition(markers[i].position);
//    }
//}
//
//void PointSchemeDrawer::draw()
//{
//
//}
//
//void PointSchemeDrawer::createMarkersCrowd(const std::vector<MarkerState>& markers)
//{
//    int count = markers.size();
//    points.resize(count);
//    for (int i = 0; i < count; ++i) {
//        Vec3 position = markers[i].position;
//        points[i] = addPoint(position);
//        node->addChild(points[i]);
//    }
//}
//
//PointSchemeDrawer::GeodePtr PointSchemeDrawer::createMarker(const osg::Vec4& color, float scale) const
//{
//    PointSchemeDrawer::GeodePtr geode = new osg::Geode();
//    ref_ptr<Geometry> geometry = new osg::Geometry();
//
//    geode->addDrawable(geometry);
//
//    ref_ptr<Vec3Array> pyramidVertices = new osg::Vec3Array;
//
//    pyramidVertices->push_back(osg::Vec3( 0.0f, 0.0f, 0.0f) * scale);
//    pyramidVertices->push_back(osg::Vec3( 1.0f, 0.0f, 0.0f) * scale);
//    pyramidVertices->push_back(osg::Vec3( 1.0f, 1.0f, 0.0f) * scale);
//    pyramidVertices->push_back(osg::Vec3( 0.0f, 1.0f, 0.0f) * scale);
//    pyramidVertices->push_back(osg::Vec3( 0.5f, 0.5f, 1.0f) * scale);
//
//    geometry->setVertexArray( pyramidVertices );
//
//    osg::DrawElementsUInt* geometryBase =
//        new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);
//    geometryBase->push_back(3);
//    geometryBase->push_back(2);
//    geometryBase->push_back(1);
//    geometryBase->push_back(0);
//    geometry->addPrimitiveSet(geometryBase);
//
//    osg::DrawElementsUInt* pyramidFaceOne =
//        new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
//    pyramidFaceOne->push_back(0);
//    pyramidFaceOne->push_back(1);
//    pyramidFaceOne->push_back(4);
//    geometry->addPrimitiveSet(pyramidFaceOne);
//
//    osg::DrawElementsUInt* pyramidFaceTwo =
//        new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
//    pyramidFaceTwo->push_back(1);
//    pyramidFaceTwo->push_back(2);
//    pyramidFaceTwo->push_back(4);
//    geometry->addPrimitiveSet(pyramidFaceTwo);
//
//    osg::DrawElementsUInt* pyramidFaceThree =
//        new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
//    pyramidFaceThree->push_back(2);
//    pyramidFaceThree->push_back(3);
//    pyramidFaceThree->push_back(4);
//    geometry->addPrimitiveSet(pyramidFaceThree);
//
//    osg::DrawElementsUInt* pyramidFaceFour =
//        new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
//    pyramidFaceFour->push_back(3);
//    pyramidFaceFour->push_back(0);
//    pyramidFaceFour->push_back(4);
//    geometry->addPrimitiveSet(pyramidFaceFour);
//
//    osg::Vec4Array* colors = new osg::Vec4Array;
//    colors->push_back(color);
//    colors->push_back(color);
//    colors->push_back(color);
//    colors->push_back(color);
//    colors->push_back(color);
//
//    geometry->setColorArray(colors);
//    geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
//
//    return geode;
//}
//
//PointSchemeDrawer::TransformPtr PointSchemeDrawer::addPoint( const osg::Vec3& point )
//{
//    float rand = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
//    Vec4 color(rand, 1, rand, 1);
//    GeodePtr marker = createMarker(color, 1.5f);
//    TransformPtr transform = new PositionAttitudeTransform();
//    transform->addChild(marker);
//    transform->setPosition(point);
//    return transform;
//}

//void LineSchemeDrawer::draw()
//{
//
//}
//
//void LineSchemeDrawer::update( double time )
//{
//    auto joints = getVisualiztionScheme().lock()->getJointsStates();
//    for (int i = joints.size() - 1; i >= 0; --i) {
//        ref_ptr<Vec3Array> vertices = buffers[i];
//        (*vertices)[0] = joints[i].from;
//        (*vertices)[1] = joints[i].to;
//        osg::Drawable* d = lines[i]->getDrawable(0);
//        osg::Geometry* g = dynamic_cast<osg::Geometry*>(d);
//
//        //std::cout << "from : (" << (*vertices)[0][0] << ", " << (*vertices)[0][1] << ", "  << (*vertices)[0][2] << ") ";
//        //std::cout << "to : (" << (*vertices)[1][0] << ", " << (*vertices)[1][1] << ", "  << (*vertices)[1][2] << ") " << endl;
//
//        g->setVertexArray(vertices);
//    }
//}
//
//void LineSchemeDrawer::deinit()
//{
//
//}
//
//void LineSchemeDrawer::init( SkeletalVisualizationSchemeWeak scheme )
//{
//    UTILS_ASSERT(scheme.lock());
//    OsgSchemeDrawer::init(scheme);
//
//    node = new osg::Group;
//    auto bones = scheme.lock()->getJointsStates();
//    
//    BOOST_FOREACH(JointState& state, bones) {
//        addLine(state.from, state.to);
//    }
//}
//
//osg::ref_ptr<osg::Node> LineSchemeDrawer::getNode()
//{
//    return node;
//}
//
//void LineSchemeDrawer::addLine( const osg::Vec3& from, const osg::Vec3& to )
//{
//    GeodePtr geode = new Geode();
//    lines.push_back(geode);
//    osg::ref_ptr<Geometry> linesGeom = new osg::Geometry();
//
//    ref_ptr<Vec3Array> vertices = new osg::Vec3Array(2);
//    buffers.push_back(vertices);
//    (*vertices)[0] = from;
//    (*vertices)[1] = to;
//
//    linesGeom->setVertexArray(vertices);
//
//    ref_ptr<Vec4Array> colors = new osg::Vec4Array;
//    colors->push_back(osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f));
//    linesGeom->setColorArray(colors);
//    linesGeom->setColorBinding(osg::Geometry::BIND_OVERALL);
//
//
//    osg::Vec3Array* normals = new osg::Vec3Array;
//    normals->push_back(osg::Vec3(1.0f, -0.0f, 0.0f));
//    linesGeom->setNormalArray(normals);
//    linesGeom->setNormalBinding(osg::Geometry::BIND_OVERALL);
//
//    linesGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, 2));
//
//    geode->addDrawable(linesGeom);
//    node->addChild(geode);
//}
