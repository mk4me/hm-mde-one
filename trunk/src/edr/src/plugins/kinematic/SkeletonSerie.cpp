#include "PCH.h"
#include "SkeletonSerie.h"

void SkeletonSerie::setData( const core::ObjectWrapperConstPtr & data )
{
	UTILS_ASSERT(data->getTypeInfo() == typeid(kinematic::JointAnglesCollection));
    this->data = data;
	connect(visualizer->actionSwitchAxes, SIGNAL(triggered(bool)), this, SLOT(setAxis(bool)));
	
	kinematic::JointAnglesCollectionConstPtr collection = data->get();
	scheme = SkeletalVisualizationSchemePtr(new SkeletalVisualizationScheme());
	scheme->setJoints(collection);
	
	skeletonDrawers->addDrawer(OsgSchemeDrawerPtr(new GlPointSchemeDrawer(DrawSkeleton, 3, 0.02f)));
	skeletonDrawers->addDrawer(OsgSchemeDrawerPtr(new GlLineSchemeDrawer(DrawSkeleton, 10, 0.005f)));
	
	transformNode = new osg::PositionAttitudeTransform();
	transformNode->addChild(skeletonDrawers->getNode());

    
	
    skeletonDrawers->init(scheme);
    
    SkeletalVisualizationSchemePtr scheme(new SkeletalVisualizationScheme());
    MarkerCollectionConstPtr mc = createTrajectories(collection);
    scheme->setMarkers(mc);
    TrajectoryDrawerPtr trajectoryDrawer(new TrajectoryDrawer(osg::Vec4(1, 1, 1, 1), 300));
    trajectoryDrawer->init(scheme);
    transformNode->addChild(trajectoryDrawer->getNode());

    visualizer->trajectoriesDialog->setMarkers(trajectoryDrawer, QString(data->getName().c_str()));
	visualizer->transformNode->addChild(transformNode);
	setAxis(true);
	//visualizer->actionSwitchAxes->trigger();
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

MarkerCollectionConstPtr SkeletonSerie::createTrajectories( kinematic::JointAnglesCollectionConstPtr joints )
{
    if (joints->getNumChannels() > 0) {
        using namespace kinematic;
        MarkerCollectionPtr markers(new MarkerCollection);
        SkeletalVisualizationScheme scheme;
        scheme.setJoints(joints);
        JointAngleChannelConstPtr joint = joints->getChannel(0);
        const std::vector<SkeletalVisualizationScheme::JointState>& states = scheme.getJointStates();
        int count = states.size();
        for (int i = 0; i < count; i++) {
            MarkerChannelPtr marker(new MarkerChannel(joint->getSamplesPerSecond()));
            std::string s("Joint");
            s += boost::lexical_cast<std::string>(i);
            marker->setName(s);
            markers->addChannel(marker);
        }
   
        
        int argumentsSize = joint->size();
        for (int arg = 0; arg < argumentsSize; arg++) {
            scheme.setTime(joint->argument(arg));

            const std::vector<SkeletalVisualizationScheme::JointState>& states = scheme.getJointStates();
            // czy kolejnosc jest zapewniona? 
            int size = states.size();
            for (int i = 0; i < size; i++) {
                MarkerChannelPtr marker = markers->getChannel(i);
                marker->addPoint(states[i].position);
            }
        }
        
        return markers;
    }

    throw std::runtime_error("Skeleton Serie: Null object passed");
}

