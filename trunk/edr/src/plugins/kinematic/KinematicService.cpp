#include "PCH.h"

#include <boost/foreach.hpp>
#include <osg/Geode>
#include <core/IServiceManager.h>
#include <core/C3DParserEx.h>
#include <plugins/timeline/ITimeline.h>
#include <core/IModel.h>
#include <core/IDataManager.h>
#include <core/IVisualizer.h>
#include <core/C3DChannels.h>
#include <plugins/timeline/Stream.h>
#include "mainwindow.h"
#include <core/Log.h>
#include <core/IService.h>
#include <boost/foreach.hpp>
#include <core/ObjectWrapper.h>
#include <boost/filesystem.hpp>
#include <QtCore/QObject>
#include <core/Log.h>
#include <QtCore/QtCore>
#include <QtGui/QtGui>
#include <QtOpenGL/QtOpenGL>
#include <QtCore/QDir>
#include <plugins/kinematic/Wrappers.h>
#include <plugins/kinematic/skeletalVisualizationScheme.h>
#include <osgGA/OrbitManipulator>
#include "SchemeDrawerContainer.h"
#include "KinematicService.h"


using namespace core;
using namespace osg;


KinematicService::KinematicService() :
    name("KinematicService")//,
    //logic(new SkeletonViewerLogic)
{
   //widget = new MainWindow();
   //// viz.createWidget();
   //SkeletalVisualizationSchemePtr visualization = SkeletalVisualizationScheme::create();
   //logic->setVisualization(visualization);
   //widget->setLogic(logic);
}

void KinematicService::loadData(IServiceManager* serviceManager, core::IDataManager* dataManager)
{
    ITimelinePtr timeline = core::queryServices<ITimeline>(serviceManager);
    if ( timeline ) {
        for (int i = stream.size() - 1; i >= 0; --i) {
            timeline->removeStream(stream[i]);
        }
        std::vector<SkeletalVisualizationSchemePtr> schemes = core::queryDataPtr(dataManager);
        for (int i = schemes.size() - 1; i >= 0; --i) {
            timeline->addStream(timeline::StreamPtr(new KinematicTimeline(schemes[i])));
        }
    } else {
        OSG_WARN<<"ITimeline not found."<<std::endl;
    }
}

void KinematicVisualizer::setUp( IObjectSource* source )
{
    resetScene();
    refillDrawersMaps();
    SkeletalVisualizationSchemeConstPtr scheme = source->getObject(0);
    
    if (scheme) {        
        if (scheme->getKinematicModel()->hasSkeleton()) {
            SchemeDrawerContainerPtr skeleton = drawersByName["Skeleton"];
            skeleton->addDrawer(OsgSchemeDrawerPtr(new LineSchemeDrawer(DrawSkeleton)));
            skeleton->addDrawer(OsgSchemeDrawerPtr(new GlLineSchemeDrawer(DrawSkeleton, 10, 0.05f)));
            skeleton->addDrawer(OsgSchemeDrawerPtr(new GlPointSchemeDrawer(DrawSkeleton, 2, 0.08f)));

            SchemeDrawerContainerPtr rectangular = drawersByName["Rectangular"];
            rectangular->addDrawer(OsgSchemeDrawerPtr(new GlLineSchemeDrawer(DrawSkeleton, 4, 0.45f, osg::Vec4(0, 1, 0, 1))));
            actionByName["Skeleton"]->setVisible(true);
            actionByName["Rectangular"]->setVisible(true);
        } else {
            actionByName["Skeleton"]->setVisible(false);
            actionByName["Rectangular"]->setVisible(false);
        }

        if (scheme->getKinematicModel()->hasMarkers()) {
            SchemeDrawerContainerPtr markers = drawersByName["Markers"];
            markers->addDrawer(OsgSchemeDrawerPtr(new GlPointSchemeDrawer(DrawMarkers, 3, 0.08f)));
            markers->addDrawer(OsgSchemeDrawerPtr(new GlLineSchemeDrawer(DrawMarkers, 10, 0.02f)));
            currentDrawer = markers;
            actionByName["Markers"]->setVisible(true);
        } else {
            actionByName["Markers"]->setVisible(false);
        }

        if (scheme->getKinematicModel()->hasMarkers() && scheme->getKinematicModel()->hasSkeleton()) {
            SchemeDrawerContainerPtr both = drawersByName["Both"];
            both->addDrawer(OsgSchemeDrawerPtr(new LineSchemeDrawer(DrawSkeleton)));
            both->addDrawer(OsgSchemeDrawerPtr(new GlLineSchemeDrawer(DrawSkeleton, 10, 0.05f)));
            both->addDrawer(OsgSchemeDrawerPtr(new GlPointSchemeDrawer(DrawSkeleton, 2, 0.08f)));
            both->addDrawer(OsgSchemeDrawerPtr(new GlPointSchemeDrawer(DrawMarkers, 3, 0.08f)));
            both->addDrawer(OsgSchemeDrawerPtr(new GlLineSchemeDrawer(DrawMarkers, 10, 0.02f)));
            currentDrawer = both;
            actionByName["Both"]->setVisible(true);
        } else {
            actionByName["Both"]->setVisible(false);
        }
               
        for (auto it = drawersByName.begin(); it != drawersByName.end(); it++) {
            (it->second)->init(scheme);
        }
        transformNode->addChild(currentDrawer->getNode());
    }
 }

IVisualizer* KinematicVisualizer::create() const
{
    return new KinematicVisualizer();
}

void KinematicVisualizer::getInputInfo( int source, std::string& name, ObjectWrapper::Types& types )
{
    if (source == 0) {
        name = "model";
        types.push_back(typeid(SkeletalVisualizationScheme));
    }
}

QWidget* KinematicVisualizer::createWidget(std::vector<QObject*>& actions)
{
    widget = new osgui::QOsgDefaultWidget();
    const osg::GraphicsContext::Traits* traits = widget->getCamera()->getGraphicsContext()->getTraits();
    widget->setTimerActive(true);
    ref_ptr<osgViewer::StatsHandler> handler = new osgViewer::StatsHandler;
    widget->addEventHandler(handler);

    widget->addEventHandler(new osgGA::StateSetManipulator(
        widget->getCamera()->getOrCreateStateSet()
        ));
    
    ref_ptr<osgGA::OrbitManipulator> cameraManipulator = new osgGA::OrbitManipulator();
    cameraManipulator->setElevation(30.0);
    cameraManipulator->setHeading(10.0);

    widget->setCameraManipulator(cameraManipulator);
    
    // stworzenie i dodanie œwiat³a przyczepionego do kamery
    ref_ptr<Light> light = new osg::Light;
    light->setLightNum(1);
    light->setPosition(osg::Vec4(0.0,0.0,0.0,1.0f));
    light->setAmbient(osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
    light->setDiffuse(osg::Vec4(1.0f,1.0f,1.0f,0.0f));
    light->setConstantAttenuation(1.0f);
    
    widget->setLight(light);
    // tworzenie kamery
    widget->getCamera()->setClearColor(osg::Vec4(0.0f, 0.1f, 0.0f, 1));

    actionSwitchAxes = new QAction("Switch Axes", widget);
    actionSwitchAxes->setCheckable(true);
    actionSwitchAxes->setChecked(false );
    connect(actionSwitchAxes, SIGNAL(triggered(bool)), this, SLOT(setAxis(bool)));
    actions.push_back(actionSwitchAxes);

    QMenu* testMenu = new QMenu("testMenu", widget);
    QActionGroup* group = new QActionGroup(widget);
    connect(group, SIGNAL(triggered(QAction *)), this, SLOT(actionTriggered(QAction*)));

    addAction(actions, "Markers", testMenu, group);
    addAction(actions, "Skeleton", testMenu, group);
    addAction(actions, "Both", testMenu, group);
    addAction(actions, "Rectangular", testMenu, group);
    actions.push_back(testMenu);


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

GeodePtr KinematicVisualizer::createFloor()
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

    ref_ptr<StateSet> stateset = new osg::StateSet;
    stateset->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    geode->setStateSet(stateset);
    
    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
    colors->push_back(osg::Vec4(0.05f, 0.3f, 0.05f, 1.0f));
    linesGeom->setColorArray(colors);
    linesGeom->setColorBinding(osg::Geometry::BIND_OVERALL);
    
    linesGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, 44));

    geode->addDrawable(linesGeom);
    return geode;
}

QIcon* KinematicVisualizer::createIcon()
{
    boost::filesystem::path p = core::getDataManager()->getResourcesPath() / "icons/3D.png";
    return new QIcon(p.string().c_str());
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