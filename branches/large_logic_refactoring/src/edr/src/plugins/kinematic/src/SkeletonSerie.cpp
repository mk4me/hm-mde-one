#include "PCH.h"
#include "SkeletonSerie.h"
#include "MarkersVisualizationScheme.h"

SkeletonSerie::SkeletonSerie( KinematicVisualizer * visualizer ) : 
    visualizer(visualizer), 
    skeletonDrawers(new SchemeDrawerContainer()),
    skeletonNode(new osg::PositionAttitudeTransform()),
    xyzAxis(false)
{

}

void SkeletonSerie::setData( const core::ObjectWrapperConstPtr & data )
{
	UTILS_ASSERT(data->getTypeInfo() == typeid(kinematic::JointAnglesCollection));
    this->data = data;
	connect(visualizer->actionSwitchAxes, SIGNAL(triggered(bool)), this, SLOT(setAxis(bool)));
	
	kinematic::JointAnglesCollectionConstPtr collection = data->get();
	scheme = SkeletalVisualizationSchemePtr(new SkeletalVisualizationScheme());
	scheme->setJoints(collection);
	
	skeletonDrawers->addDrawer(OsgSchemeDrawerPtr(new GlPointSchemeDrawer(3, 0.02f)));
	skeletonDrawers->addDrawer(OsgSchemeDrawerPtr(new GlLineSchemeDrawer(10, 0.005f)));
    
    trajectoryDrawer.reset(new TrajectoryDrawer(osg::Vec4(1, 1, 1, 0.33f), 300));
    MarkersVisualizationSchemePtr tempScheme(new MarkersVisualizationScheme());
    MarkerCollectionConstPtr mc = createTrajectories(collection);
    tempScheme->setMarkers(mc);
    trajectoryDrawer->init(tempScheme);
    skeletonDrawers->addDrawer(trajectoryDrawer);
	
    skeletonDrawers->init(scheme);
    skeletonNode->addChild(skeletonDrawers->getNode());
    //skeletonNode->addChild(trajectoryDrawer->getNode());
    
    visualizer->trajectoriesDialog->setDrawer(skeletonDrawers, QString::fromStdString(data->getName()));
    visualizer->schemeDialog->setDrawer(skeletonDrawers, QString::fromStdString(data->getName()));
    transformNode->addChild(skeletonNode);
	//visualizer->transformNode->addChild(skeletonNode);
	setAxis(true);
	//visualizer->actionSwitchAxes->trigger();
    matrixTransform->setMatrix(getInitialMatrix());
    trajectoryDrawer->setOffset(-scheme->getRootPosition(0.0));
}

void SkeletonSerie::setAxis( bool xyz)
{
    xyzAxis = xyz;
	if (xyz) {
		osg::Quat q(osg::PI_2, osg::Vec3(1.0f, 0.0f, 0.0f));
		osg::Quat q2(osg::PI_2, osg::Vec3(0.0f, 0.0f, 1.0f));
		skeletonNode->setAttitude(q * q2);
	} else {
		osg::Quat q;
		skeletonNode->setAttitude(q);
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
        const std::vector<VisualizationScheme::State>& s = scheme.getStates();
        int count = s.size();
        for (int i = 0; i < count; ++i) {
            MarkerChannelPtr marker(new MarkerChannel(joint->getSamplesPerSecond()));
            marker->setName(s[i].name);
            markers->addChannel(marker);
        }
   
        int argumentsSize = joint->size();
        for (int arg = 0; arg < argumentsSize; ++arg) {
            auto time = joint->argument(arg);
            scheme.setTime(time);
            auto rootPos = scheme.getCurrentPosition();
            const std::vector<VisualizationScheme::State>& states = scheme.getStates();
            // czy kolejność jest zapewniona? 
            int size = states.size();
            for (int i = 0; i < size; ++i) {
                VectorChannelPtr marker = markers->getChannel(i);
                marker->addPoint(states[i].position + rootPos);
            }
        }
        
        return markers;
    }

    throw std::runtime_error("Skeleton Serie: Null object passed");
}

osg::Matrix SkeletonSerie::getXYZMatrix() const
{
    osg::Quat q(osg::PI_2, osg::Vec3(1.0f, 0.0f, 0.0f));
    osg::Quat q2(osg::PI_2, osg::Vec3(0.0f, 0.0f, 1.0f));
    osg::Matrix r; 
    r.setRotate(q * q2);
    return r;
}

void SkeletonSerie::setLocalTime( double time )
{
    UTILS_ASSERT(scheme && skeletonDrawers);
    auto shift = scheme->getCurrentPosition();
    scheme->setTime(time);
    skeletonDrawers->update();
    shift = scheme->getCurrentPosition() - shift;
    osg::Matrix m = matrixTransform->getMatrix();
    osg::Matrix r = xyzAxis ? getXYZMatrix() : osg::Matrix();

    osg::Matrix r2 = m;
    r2.setTrans(osg::Vec3());
    //m.setTrans(scheme->getCurrentPosition() * r);

    trajectoryDrawer->setOffset(-scheme->getCurrentPosition());
    m.setTrans(m.getTrans() + shift * r * r2);
    matrixTransform->setMatrix(m);
}

osg::Matrix SkeletonSerie::getInitialMatrix() const
{
  osg::Matrix m;

  osg::Matrix r = xyzAxis ? getXYZMatrix() : osg::Matrix();
  m.setTrans(scheme->getRootPosition(0.0) * r);
  return m;
}

void SkeletonSerie::resetTransform()
{
    MatrixTransformPtr transform = getMatrixTransformNode();
    transform->setMatrix(getInitialMatrix());
    scheme->setNormalizedTime(0.0);
    setLocalTime(getTime());
}

void SkeletonSerie::setName( const std::string & name )
{
    this->name = name;
}

const std::string & SkeletonSerie::getName() const
{
    return name;
}

const core::ObjectWrapperConstPtr & SkeletonSerie::getData() const
{
    return data;
}

double SkeletonSerie::getLength() const
{
    UTILS_ASSERT(scheme);
    return scheme->getDuration();
}


