#include "PCH.h"
#include "AbstractSkeletonSerie.h"
#include <osgutils/OsgSchemeDrawer.h>
#include "TrajectoriesDrawer.h"
#include "SkeletalVisualizationSchemeHelper.h"
#include <threadingUtils/StreamData.h>
#include <kinematiclib/SkeletonState.h>
#include "osgManipulator/TranslateAxisDragger"
#include <osg/ComputeBoundsVisitor>
#include <osg/LightModel>

static const osg::Quat invQXYZ = osg::Quat(osg::PI_2, osg::Vec3(1.0f, 0.0f, 0.0f)) * osg::Quat(osg::PI_2, osg::Vec3(0.0f, 0.0f, 1.0f));

AbstractSkeletonSerie::AbstractSkeletonSerie(KinematicVisualizer * visualizer,
	const utils::TypeInfo & requestedType,
	const core::VariantConstPtr & data) :
	visualizer(visualizer),
	data(data), requestedType(requestedType),
	lastUpdateTime(std::numeric_limits<double>::min()),
	xyzAxis(false),
	pointsDrawer(new osgutils::PointsDrawer(15)),
	connectionsDrawer(new osgutils::ConnectionsSphereDrawer(3)),
	localRootNode(new osg::PositionAttitudeTransform)
{
}

void AbstractSkeletonSerie::init(double ratio, int pointsCount,
	kinematic::SkeletonPtr skeleton, const kinematic::LinearizedSkeleton::GlobalMapping& mapping)
{
	this->skeleton = skeleton;
	this->nodesMapping = mapping;
	//joint2Index = kinematic::SkeletonState::createJoint2IndexMapping(*state, mapping);
	localRootNode->setScale(osg::Vec3(ratio, ratio, ratio));
	pointsDrawer->init(pointsCount);
	pointsDrawer->setSize(0.02 / ratio);
	pointsDrawer->setColor(osg::Vec4(1.0, 1.0, 0.0, 1.0));
	//localRootNode->addChild(pointsDrawer->getNode());

	//inicjalizacja połaczeń na bazie indeksów jointów
	kinematic::LinearizedSkeleton::Visitor::globalIndexedVisit(*skeleton, [this, &mapping]
		(kinematic::Skeleton::JointPtr joint, const kinematic::LinearizedSkeleton::NodeIDX idx)
	{
		for (const auto & c : joint->children())
		{
			osgutils::SegmentDescriptor sd;
			sd.length = c->value().localPosition().length();
			sd.range.first = idx;
			sd.range.second = mapping.data().right.find(c->value().name())->get_left();
			connections.push_back(sd);
		}
	});

	connectionsDrawer->init(connections);
	connectionsDrawer->setColor(osg::Vec4(0.7, 0.7, 0.7, 0.5));
	connectionsDrawer->setSize(0.005);
	localRootNode->addChild(connectionsDrawer->getNode());
	localRootNode->computeLocalToWorldMatrix(lToW, nullptr);

	matrixTransform->addChild(localRootNode);

	//setTime(0.0);

	rootPosition = osg::Vec3();// skeletonWithStates->getRootPosition(0.0);
	bodyPlanesPAT = new osg::PositionAttitudeTransform;
	update();

	//bodyplanes
	initBodyPlanes();

	//setAxis(true);

	pointsAxesDrawer.setLength(0.1);
	pointsAxesDrawer.init(skeleton->root());
	pointsAxesDrawer.setVisible(false);
	localRootNode->addChild(pointsAxesDrawer.getNode());
}

void AbstractSkeletonSerie::initBodyPlanes()
{
	//bounding box
	osg::ComputeBoundsVisitor cbv;	
	localRootNode->accept(cbv);
	osg::BoundingBox bb = cbv.getBoundingBox();	

	//powiększamy bounding box
	auto distX = (bb.xMax() - bb.xMin()) * 0.1;
	auto distY = (bb.yMax() - bb.yMin()) * 0.1;
	auto distZ = (bb.zMax() - bb.zMin()) * 0.2;

	// geoda
	osg::ref_ptr<osg::Geode> groot = new osg::Geode;

	//geometria sciany prostopadłej dla osi X
	// w prawo
	{
		osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
		vertices->push_back(osg::Vec3(0.0f, bb.yMax() + distY, bb.zMax() + distZ));
		vertices->push_back(osg::Vec3(0.0f, bb.yMin() - distY, bb.zMax() + distZ));
		vertices->push_back(osg::Vec3(0.0f, bb.yMin() - distY, bb.zMin() - distZ));
		vertices->push_back(osg::Vec3(0.0f, bb.yMax() + distY, bb.zMin() - distZ));

		osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
		normals->push_back(osg::Vec3(1.0f, 0.0f, 0.0f));

		osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
		colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 0.7f));

		osg::ref_ptr<osg::Geometry> quad = new osg::Geometry;
		quad->setVertexArray(vertices.get());
		quad->setNormalArray(normals.get());
		quad->setNormalBinding(osg::Geometry::BIND_OVERALL);
		quad->setColorArray(colors.get());
		quad->setColorBinding(osg::Geometry::BIND_OVERALL);

		quad->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, 4));
		groot->addDrawable(quad.get());
	}

	//geometria sciany prostopadłej dla osi Y
	// do przodu
	{
		osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
		vertices->push_back(osg::Vec3(bb.xMax() + distX, 0.0f, bb.zMax() + distZ));
		vertices->push_back(osg::Vec3(bb.xMin() - distX, 0.0f, bb.zMax() + distZ));
		vertices->push_back(osg::Vec3(bb.xMin() - distX, 0.0f, bb.zMin() - distZ));
		vertices->push_back(osg::Vec3(bb.xMax() + distX, 0.0f, bb.zMin() - distZ));

		osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
		normals->push_back(osg::Vec3(0.0f, 1.0f, 0.0f));

		osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
		colors->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 0.7f));

		osg::ref_ptr<osg::Geometry> quad = new osg::Geometry;
		quad->setVertexArray(vertices.get());
		quad->setNormalArray(normals.get());
		quad->setNormalBinding(osg::Geometry::BIND_OVERALL);
		quad->setColorArray(colors.get());
		quad->setColorBinding(osg::Geometry::BIND_OVERALL);

		quad->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, 4));
		groot->addDrawable(quad.get());
	}

	//geometria sciany prostopadłej dla osi Z
	{
		osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
		vertices->push_back(osg::Vec3(bb.xMax() + distX, bb.yMax() + distY, 0.0f));
		vertices->push_back(osg::Vec3(bb.xMax() + distX, bb.yMin() - distY, 0.0f));
		vertices->push_back(osg::Vec3(bb.xMin() - distX, bb.yMin() - distY, 0.0f));
		vertices->push_back(osg::Vec3(bb.xMin() - distX, bb.yMax() + distY, 0.0f));

		osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
		normals->push_back(osg::Vec3(0.0f, 0.0f, 1.0f));

		osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
		colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 0.7f));

		osg::ref_ptr<osg::Geometry> quad = new osg::Geometry;
		quad->setVertexArray(vertices.get());
		quad->setNormalArray(normals.get());
		quad->setNormalBinding(osg::Geometry::BIND_OVERALL);
		quad->setColorArray(colors.get());
		quad->setColorBinding(osg::Geometry::BIND_OVERALL);

		quad->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, 4));
		groot->addDrawable(quad.get());
	}

	// Create and set up a state set using the texture from above:
	osg::StateSet* gstateSet = new osg::StateSet();
	groot->setStateSet(gstateSet);	

	// For this state set, turn blending on (so alpha texture looks right)
	gstateSet->setMode(GL_BLEND, osg::StateAttribute::ON);

	// Disable depth testing so geometry is draw regardless of depth values
	// of geometry already draw.
	gstateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
	gstateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

	// Need to make sure this geometry is draw last. RenderBins are handled
	// in numerical order so set bin number to 11
	//gstateSet->setRenderBinDetails(11, "RenderBin");

	osg::LightModel* lm = new osg::LightModel();
	lm->setTwoSided(true);

	gstateSet->setAttribute(lm);

	bodyPlanesSwitch = new osg::Switch;
	bodyPlanesPAT->addChild(groot);
	bodyPlanesSwitch->addChild(bodyPlanesPAT);
	localRootNode->addChild(bodyPlanesSwitch);
	bodyPlanesSwitch->setNodeMask(0);
}


void AbstractSkeletonSerie::setAxis(bool xyz)
{
	//TODO - sprawdzic jak to dziala
	//localRootNode->setAttitude(xyz == true ? invQXYZ : osg::Quat());

	//localRootNode->computeLocalToWorldMatrix(lToW, nullptr);
}


const core::VariantConstPtr & AbstractSkeletonSerie::getData() const
{
	return data;
}


const osg::Vec3 AbstractSkeletonSerie::pivotPoint() const
{
	return rootPosition * lToW;
}

kinematic::SkeletonPtr AbstractSkeletonSerie::getSkeleton()
{
	return skeleton;
}

void AbstractSkeletonSerie::update()
{
	std::vector<osg::Vec3> pos;
	pos.reserve(nodesMapping.data().size());
		
	kinematic::LinearizedSkeleton::Visitor::visit(*skeleton, [&pos](kinematic::Skeleton::JointConstPtr node)
	{
		pos.push_back(node->value().globalPosition());
	});

	pointsDrawer->update(pos);
	connectionsDrawer->update(pos);
	pointsAxesDrawer.update();
	bodyPlanesPAT->setPosition(pos[0]);
	bodyPlanesPAT->setAttitude(skeleton->root()->value().globalOrientation());
}


const utils::TypeInfo & AbstractSkeletonSerie::getRequestedDataType() const
{
	return requestedType;
}

void AbstractSkeletonSerie::setJointsOrientationsVisible()
{
	pointsAxesDrawer.setVisible(!pointsAxesDrawer.isVisible());
}

void AbstractSkeletonSerie::setBodyPlanesVisible()
{
	bodyPlanesSwitch->setNodeMask(~(bodyPlanesSwitch->getNodeMask()));
}


void PointsOrientationsDrawer::init(kinematic::Skeleton::JointConstPtr root)
{
	if (pointAxes.empty()) {
		auto visitor = [&](kinematic::Skeleton::JointConstPtr node, kinematic::Skeleton::Joint::size_type level)
		{
			osg::ref_ptr<osgManipulator::TranslateAxisDragger> ne = new osgManipulator::TranslateAxisDragger();
			ne->setupDefaultGeometry();
			pointAxes.push_back(ne);
			localNode->addChild(ne);
		};

		kinematic::LinearizedSkeleton::Visitor::VisitOrder::NodeVisitOrder::visit(root, visitor);
		this->root = root;
		setVisible(visible);
	} else {
		throw core::runtime_error("Cannot call init method twice");
	}
}

osg::ref_ptr<osg::Switch> PointsOrientationsDrawer::getNode()
{
	return localNode;
}

void PointsOrientationsDrawer::update()
{
	auto root = this->root.lock();
	if (root) {
		auto it = pointAxes.begin();
		auto visitor = [&](kinematic::Skeleton::JointConstPtr node, kinematic::Skeleton::Joint::size_type level)
		{
			auto ne = *(it++);
			osg::Matrix mat;
			mat.set(node->value().globalOrientation());
			mat.setTrans(node->value().globalPosition());
			mat.preMultScale(osg::Vec3(scale, scale, scale));
			ne->setMatrix(mat);
		};
		kinematic::LinearizedSkeleton::Visitor::VisitOrder::NodeVisitOrder::visit(root, visitor);
	}
}

void PointsOrientationsDrawer::setLength(double length)
{
	this->scale = length;
}

void PointsOrientationsDrawer::setVisible(bool visible)
{
	this->visible = visible;
	if (visible == true) {
		localNode->setAllChildrenOn();
	} else {
		localNode->setAllChildrenOff();
	}

}

bool PointsOrientationsDrawer::isVisible()
{
	return this->visible;
}
