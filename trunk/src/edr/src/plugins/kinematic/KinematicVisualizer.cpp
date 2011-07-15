#include "PCH.h"
#include "KinematicVisualizer.h"

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

    input.name = "model";
    input.type = typeid(SkeletalVisualizationScheme);
    input.required = true;
    input.modify = false;

    info.push_back(input);
}

int KinematicVisualizer::getMaxDataSeries() const
{
    return 1;
}

core::IVisualizer::SerieBase *KinematicVisualizer::createSerie(const core::ObjectWrapperConstPtr & data, const std::string & name)
{
    core::IVisualizer::SerieBase * ret = new KinematicVisualizerSerie(this);
    ret->setName(name);
    ret->setData(data);

    return ret;
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

    osg::ref_ptr<osgGA::OrbitManipulator> cameraManipulator = new osgGA::OrbitManipulator();
    cameraManipulator->setElevation(30.0);
    cameraManipulator->setHeading(10.0);

    widget->setCameraManipulator(cameraManipulator);

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

    actionSwitchAxes = new QAction("Switch Axes", widget);
    actionSwitchAxes->setCheckable(true);
    actionSwitchAxes->setChecked(false );
    connect(actionSwitchAxes, SIGNAL(triggered(bool)), this, SLOT(setAxis(bool)));
    actions.push_back(actionSwitchAxes);

    QMenu* visualizationMenu = new QMenu("Visualization", widget);
    QActionGroup* group = new QActionGroup(widget);
    connect(group, SIGNAL(triggered(QAction *)), this, SLOT(actionTriggered(QAction*)));

    addAction(actions, "Markers", visualizationMenu, group);
    addAction(actions, "Skeleton", visualizationMenu, group);
    addAction(actions, "Both", visualizationMenu, group);
    addAction(actions, "Rectangular", visualizationMenu, group);
    actions.push_back(visualizationMenu);


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

KinematicVisualizer::GeodePtr KinematicVisualizer::createFloor()
{
    GeodePtr geode = new osg::Geode();
    // todo sparametryzowac
    osg::ref_ptr<osg::Geometry> linesGeom = new osg::Geometry();

    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(44);
    osg::Vec3 v;
    for (int i = 0; i < 11; i++) {
        v.set(-10.0f + 2*i, -10.0f, 0);
        (*vertices)[2 * i] = v;      
        v.set(-10.0f + 2*i, 10.0f, 0);
        (*vertices)[2 * i + 1] = v;   
        v.set(-10.0f, -10.0f + 2*i, 0);
        (*vertices)[22 + 2 * i] = v;   
        v.set(10.0f, -10.0f + 2*i, 0);
        (*vertices)[22 + 2 * i + 1] = v;

    }

    linesGeom->setVertexArray(vertices);

    osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
    stateset->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    geode->setStateSet(stateset);

    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
    colors->push_back(osg::Vec4(0.05f, 0.05f, 0.3f, 1.0f));
    linesGeom->setColorArray(colors);
    linesGeom->setColorBinding(osg::Geometry::BIND_OVERALL);

    linesGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, 44));

    geode->addDrawable(linesGeom);
    return geode;
}

QIcon* KinematicVisualizer::createIcon()
{
    return new QIcon(core::getResourceString("icons/3D.png"));
}

void KinematicVisualizer::setAxis( bool xyz )
{
    if (xyz) {
        osg::Quat q(osg::PI_2, osg::Vec3(1.0f, 0.0f, 0.0f));
        transformNode->setAttitude(q);
    } else {
        osg::Quat q;
        transformNode->setAttitude(q);
    }
}

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

void KinematicVisualizer::addAction(std::vector<QObject*>& actions, const std::string& name, QMenu* menu, QActionGroup* group )
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