#include "PCH.h"
#include "KinematicVisualizer.h"
#include "GRFSerie.h"
#include "SkeletonSerie.h"
#include "MarkerSerie.h"

void KinematicVisualizer::setUp( core::IObjectSource* source )
{
    reset();
}

void KinematicVisualizer::reset()
{
    resetScene();
    refillDrawersMaps();
}

core::IVisualizer* KinematicVisualizer::createClone() const
{
    return new KinematicVisualizer();
}

void KinematicVisualizer::getInputInfo( std::vector<core::IInputDescription::InputInfo>& info)
{
    core::IInputDescription::InputInfo input;

	input.name = "markers";
	input.type = typeid(MarkerCollection);
	input.required = false;
	input.modify = false;

	info.push_back(input);

	input.name = "grf";
	input.type = typeid(GRFCollection);
	input.required = false;
	input.modify = false;

    info.push_back(input);

	input.name = "skeleton";
	input.type = typeid(kinematic::JointAnglesCollection);
	input.required = false;
	input.modify = false;

	info.push_back(input);
}

int KinematicVisualizer::getMaxDataSeries() const
{
    return -1;
}

core::IVisualizer::TimeSerieBase *KinematicVisualizer::createSerie(const core::ObjectWrapperConstPtr & data, const std::string & name)
{
	core::IVisualizer::TimeSerieBase * ret = nullptr;
	if (data->getTypeInfo() == typeid(GRFCollection)) {
		ret = new GRFSerie(this);
		ret->setName(name + "_grf");
		ret->setData(data);
	} else if (data->getTypeInfo() == typeid (MarkerCollection)) {
		ret = new MarkerSerie(this);
		ret->setName(name + "_markers");
		ret->setData(data);
	} else if (data->getTypeInfo() == typeid (kinematic::JointAnglesCollection)) {
		ret = new SkeletonSerie(this);
		ret->setName(name + "_skeleton");
		ret->setData(data);
	} else {
		UTILS_ASSERT(false);
	}
    return ret;
}

core::IVisualizer::TimeSerieBase * KinematicVisualizer::createSerie(const core::IVisualizer::SerieBase* serie)
{
    return nullptr;
}

void KinematicVisualizer::removeSerie(core::IVisualizer::SerieBase *serie)
{
    reset();
}

QWidget* KinematicVisualizer::createWidget(std::vector<QObject*>& actions)
{
    widget = new osgui::QOsgDefaultWidget();
    const osg::GraphicsContext::Traits* traits = widget->getCamera()->getGraphicsContext()->getTraits();
    widget->setTimerActive(true);
    osg::ref_ptr<osgViewer::StatsHandler> handler = new osgViewer::StatsHandler;
    widget->addEventHandler(handler);

    widget->addEventHandler(new osgGA::StateSetManipulator(
        widget->getCamera()->getOrCreateStateSet()
        ));

    cameraManipulator = new osgGA::OrbitManipulator();
    widget->setCameraManipulator(cameraManipulator);
	setLeft();
    // stworzenie i dodanie œwiat³a przyczepionego do kamery
    osg::ref_ptr<osg::Light> light = new osg::Light;
    light->setLightNum(1);
    light->setPosition(osg::Vec4(0.0,0.0,0.0,1.0f));
    light->setAmbient(osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
    light->setDiffuse(osg::Vec4(1.0f,1.0f,1.0f,0.0f));
    light->setConstantAttenuation(1.0f);

    widget->setLight(light);
    // tworzenie kamery
    widget->getCamera()->setClearColor(osg::Vec4(0.0f, 0.0f, 0.1f, 1));

    QIcon icon0;
    icon0.addFile(QString::fromUtf8(":/resources/icons/switch-axisa.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
    icon0.addFile(QString::fromUtf8(":/resources/icons/switch-axis.png"), QSize(), QIcon::Mode::Normal, QIcon::State::On);
    actionSwitchAxes = new QAction("Switch Axes", widget);
    actionSwitchAxes->setIcon(icon0);
    actionSwitchAxes->setCheckable(true);
    actionSwitchAxes->setChecked(false );
    actionSwitchAxes->setVisible(false);
    //connect(actionSwitchAxes, SIGNAL(triggered(bool)), this, SLOT(setAxis(bool)));
    actions.push_back(actionSwitchAxes);

    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/resources/icons/tracea.png"));
	actionTrajectories = new QAction("Trajectories", widget);
    actionTrajectories->setIcon(icon1);
    actionTrajectories->setCheckable(true);
    actionTrajectories->setVisible(false);

    QIcon icon2;
    icon2.addFile(QString::fromUtf8(":/resources/icons/skeletal_tracea.png"));
    actionGhost = new QAction("Ghost", widget);
    actionGhost->setIcon(icon2);
    actionGhost->setCheckable(true);
    actionGhost->setVisible(false);
	actions.push_back(actionTrajectories);
    actions.push_back(actionGhost);
	
    QIcon icon3;
    icon3.addFile(QString::fromUtf8(":/resources/icons/viewa.png"));
    QMenu* viewMenu = new QMenu("View", widget);
    viewMenu->setIcon(icon3);

	QAction* lft_action = viewMenu->addAction("Left"); 
	QAction* rht_action = viewMenu->addAction("Right"); 
	QAction* frn_action = viewMenu->addAction("Front"); 
	QAction* bck_action = viewMenu->addAction("Back"); 
	QAction* top_action = viewMenu->addAction("Top"); 
	QAction* btm_action = viewMenu->addAction("Bottom"); 
	
	connect(lft_action, SIGNAL(triggered()), this, SLOT(setLeft()));
	connect(rht_action, SIGNAL(triggered()), this, SLOT(setRight())); 
	connect(frn_action, SIGNAL(triggered()), this, SLOT(setFront())); 
	connect(bck_action, SIGNAL(triggered()), this, SLOT(setBehind()));
	connect(top_action, SIGNAL(triggered()), this, SLOT(setTop()));
	connect(btm_action, SIGNAL(triggered()), this, SLOT(setBottom())); 	
	actions.push_back(viewMenu);
	
    osg::Vec3 pos (0.0f, 9.0f, 3.0f);
    osg::Vec3 up(0,0,1);

    osg::Vec3 at;
    widget->getCamera()->setViewMatrixAsLookAt(pos, at, up);
    rootNode = new osg::Group;
    rootNode->setUpdateCallback(new KinematicNotifier(this));
    rootNode->addChild(createFloor());
    widget->setSceneData(rootNode);
    return widget;
}

const std::string& KinematicVisualizer::getName() const
{
    return name;
}

KinematicVisualizer::KinematicVisualizer() :
    name("KinematicVisualizer")
{

}

void KinematicVisualizer::update( double deltaTime )
{

}

void KinematicVisualizer::updateAnimation()
{
    if (currentDrawer) {
        currentDrawer->update();
    }
}

osg::ref_ptr<osg::Group> KinematicVisualizer::createFloor()
{
	osg::ref_ptr<osg::Group> group = new osg::Group;
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
    stateset->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(0.05f, 0.05f, 0.3f, 1.0f));

    GeodePtr geode = new osg::Geode();
    // todo sparametryzowac
    osg::ref_ptr<osg::Geometry> linesGeom = new osg::Geometry();

	float length = 3.5f;

    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(44);
    osg::Vec3 v;
    for (int i = 0; i < 11; i++) {
        v.set(-length + 2 * (i / 10.0f) * length, -length, 0);
        (*vertices)[2 * i] = v;      
        v.set(-length + 2 * (i / 10.0f) * length, length, 0);
        (*vertices)[2 * i + 1] = v;   
        v.set(-length, -length + 2 * (i / 10.0f) * length, 0);
        (*vertices)[22 + 2 * i] = v;   
        v.set(length, -length + 2 * (i / 10.0f) * length, 0);
        (*vertices)[22 + 2 * i + 1] = v;
    }

    linesGeom->setVertexArray(vertices);
    geode->setStateSet(stateset);
    linesGeom->setColorArray(colors);
    linesGeom->setColorBinding(osg::Geometry::BIND_OVERALL);

    linesGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, 44));

    geode->addDrawable(linesGeom);
	group->addChild(geode);

    return group;
}

QIcon* KinematicVisualizer::createIcon()
{
    return new QIcon(QString::fromUtf8(":/resources/icons/3D.png"));
}

//void KinematicVisualizer::setAxis( bool xyz )
//{
//    /*if (xyz) {
//        osg::Quat q(osg::PI_2, osg::Vec3(1.0f, 0.0f, 0.0f));
//        transformNode->setAttitude(q);
//    } else {
//        osg::Quat q;
//        transformNode->setAttitude(q);
//    }*/
//}

void KinematicVisualizer::actionTriggered( QAction* action )
{
    resetScene();
    currentDrawer = drawersByAction[action];
    transformNode->addChild(currentDrawer->getNode());
}

void KinematicVisualizer::resetScene()
{
    if (rootNode) {
        for (int i = rootNode->getNumChildren() - 1; i >= 0; --i) {
            rootNode->removeChild(i);
        }
    }

    transformNode = new osg::PositionAttitudeTransform();
    rootNode->addChild(createFloor());
    rootNode->addChild(transformNode);
}

void KinematicVisualizer::addAction(const std::string& name, QMenu* menu, QActionGroup* group)
{
    QAction* act = menu->addAction(name.c_str());

    actionByName[name] = act;
    act->setCheckable(true);
    group->addAction(act); 
}

void KinematicVisualizer::refillDrawersMaps()
{
    drawersByAction.clear();
    drawersByName.clear();
    for (std::map<std::string, QAction*>::iterator it = actionByName.begin(); it != actionByName.end(); it++) {
        SchemeDrawerContainerPtr drawer(new SchemeDrawerContainer());
        drawersByAction[it->second] = drawer;
        drawersByName[it->first] = drawer;
    }
}

osg::Node* KinematicVisualizer::debugGetLocalSceneRoot()
{
    return rootNode;
}

void KinematicVisualizer::setRight()
{
	this->cameraManipulator->setHeading(osg::DegreesToRadians(-90.0));
	this->cameraManipulator->setElevation(0);
	this->cameraManipulator->setDistance(8);
}

void KinematicVisualizer::setLeft()
{
	this->cameraManipulator->setHeading(osg::DegreesToRadians(90.0));
	this->cameraManipulator->setElevation(0);
	this->cameraManipulator->setDistance(8);
}

void KinematicVisualizer::setFront()
{
	this->cameraManipulator->setHeading(0);
	this->cameraManipulator->setElevation(0);
	this->cameraManipulator->setDistance(8);
}

void KinematicVisualizer::setBehind()
{
	this->cameraManipulator->setHeading(osg::DegreesToRadians(180.0));
	this->cameraManipulator->setElevation(0);
	this->cameraManipulator->setDistance(8);
}

void KinematicVisualizer::setBottom()
{
	this->cameraManipulator->setHeading(0);
	this->cameraManipulator->setElevation(osg::DegreesToRadians(-90.0));
	this->cameraManipulator->setDistance(8);
}

void KinematicVisualizer::setTop()
{
	this->cameraManipulator->setHeading(0);
	this->cameraManipulator->setElevation(osg::DegreesToRadians(90.0));
	this->cameraManipulator->setDistance(8);
}




