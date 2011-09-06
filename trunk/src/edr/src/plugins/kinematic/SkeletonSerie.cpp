#include "PCH.h"
#include "SkeletonSerie.h"

void SkeletonSerie::setSerieData( const core::ObjectWrapperConstPtr & data )
{
	UTILS_ASSERT(data->getTypeInfo() == typeid(kinematic::JointAnglesCollection));
	connect(visualizer->actionSwitchAxes, SIGNAL(triggered(bool)), this, SLOT(setAxis(bool)));
	kinematic::JointAnglesCollectionConstPtr collection = data->get();
	scheme = SkeletalVisualizationScheme::create();
	scheme->setJoints(collection);
	
	skeletonDrawers->addDrawer(OsgSchemeDrawerPtr(new GlPointSchemeDrawer(DrawSkeleton, 3, 0.02f)));
	skeletonDrawers->addDrawer(OsgSchemeDrawerPtr(new GlLineSchemeDrawer(DrawSkeleton, 10, 0.005f)));
	
	transformNode = new osg::PositionAttitudeTransform();
	transformNode->addChild(skeletonDrawers->getNode());
	
	skeletonDrawers->init(scheme);
	visualizer->transformNode->addChild(transformNode);
}

void SkeletonSerie::setAxis( bool xyz)
{
	if (xyz) {
		osg::Quat q(osg::PI_2, osg::Vec3(1.0f, 0.0f, 0.0f));
		osg::Quat q2(osg::PI_2, osg::Vec3(0.0f, 0.0f, 1.0f));
		transformNode->setAttitude(q * q2);
	} else {
		osg::Quat q;
		transformNode->setAttitude(q);
	}
}

