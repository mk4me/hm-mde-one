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
#include <core/Log.h>
#include <core/ObjectWrapper.h>
#include <QtCore/QtCore>
#include <QtGui/QtGui>
#include <QtOpenGL/QtOpenGL>
#include <QtCore/QDir>
#include <plugins/kinematic/Wrappers.h>
#include <plugins/kinematic/skeletalVisualizationScheme.h>
#include <osgGA/OrbitManipulator>
#include "ISchemeDrawer.h"
#include "OsgSchemeDrawer.h"
#include "LineSchemeDrawer.h"
#include "PointSchemeDrawer.h"
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
    if (rootNode) {
        for (int i = rootNode->getNumChildren() - 1; i >= 0; --i) {
            rootNode->removeChild(i);
        }
    }

    rootNode->addChild(createFloor());

    SkeletalVisualizationSchemeConstPtr scheme = source->getObject(0);
    
    if (scheme) {
        drawers.clear();
        OsgSchemeDrawerPtr drawer1(new PointSchemeDrawer(DrawSkeleton));
        OsgSchemeDrawerPtr drawer2(new PointSchemeDrawer(DrawMarkers));
        OsgSchemeDrawerPtr drawer3(new LineSchemeDrawer(DrawSkeleton));
        OsgSchemeDrawerPtr drawer4(new ConeDrawer(DrawMarkers));
        OsgSchemeDrawerPtr drawer5(new ConeDrawer(DrawSkeleton));
        drawers.push_back(drawer1);
        drawers.push_back(drawer2);
        drawers.push_back(drawer3);
        drawers.push_back(drawer4);
        drawers.push_back(drawer5);
        for (unsigned int i = 0; i < drawers.size(); i++) {
            drawers[i]->init(scheme);
            rootNode->addChild(drawers[i]->getNode());
        }
    }
 }

IVisualizer* KinematicVisualizer::create() const
{
    return new KinematicVisualizer();
}

void KinematicVisualizer::getSlotInfo( int source, std::string& name, ObjectWrapper::Types& types )
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
    osgGA::OrbitManipulator *cameraManipulator = new osgGA::OrbitManipulator();
    cameraManipulator->setElevation(30.0);
    cameraManipulator->setHeading(10.0);
    widget->setCameraManipulator(cameraManipulator);
    //widget->setMinimumSize(100, 100);

    // stworzenie i dodanie œwiat³a przyczepionego do kamery
    ref_ptr<Light> light = new osg::Light;
    light->setLightNum(1);
    light->setPosition(osg::Vec4(0.0,0.0,0.0,1.0f));
    light->setAmbient(osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
    light->setDiffuse(osg::Vec4(1.0f,1.0f,1.0f,0.0f));
    light->setConstantAttenuation(1.0f);
    /*light->setLinearAttenuation(1.0f/MODEL_SIZE);
    light->setQuadraticAttenuation(1.0f/osg::square(MODEL_SIZE));*/

    widget->setLight(light);
    // tworzenie kamery
    widget->getCamera()->setClearColor(osg::Vec4(0.0f, 0.1f, 0.0f, 1));
    //widget->getCamera()->setNearFarRatio(0.000000001);
    /*osg::Vec3 pos( 0.0f, 0.0f, 9.0f);
    osg::Vec3 up(0, 1, 0);*/
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
    for (int i = drawers.size() - 1; i >= 0; --i) {
        drawers[i]->update();
    }
    //if (scheme) {
    //    double interval = widget->getTimerInterval() / 1000.0;
    //    double duration = scheme->getDuration();
    //    double time = scheme->getNormalizedTime() * duration;
    //    time += interval;
    //    if (time >= duration) {
    //        time = 0.0;
    //    } else {
    //        time = time / duration;
    //    }
    //    scheme->setNormalizedTime(time);
    //}
}

GeodePtr KinematicVisualizer::createFloor()
{
    GeodePtr geode = new osg::Geode();
    // todo sparametryzowac
    osg::ref_ptr<osg::Geometry> linesGeom = new osg::Geometry();

    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(44);
    
    for (int i = 0; i < 11; i++) {
        /*(*vertices)[2 * i] = osg::Vec3(-5.0f + i, 0, -5.0f);
        (*vertices)[2 * i + 1] = osg::Vec3(-5.0f + i, 0, 5.0f);
        (*vertices)[20 + 2 * i] = osg::Vec3(-5.0f, 0, -5.0f + i);
        (*vertices)[20 + 2 * i + 1] = osg::Vec3(5.0f, 0, -5.0f + i);*/
        (*vertices)[2 * i] = osg::Vec3(-10.0f + 2*i, -10.0f, 0);
        (*vertices)[2 * i + 1] = osg::Vec3(-10.0f + 2*i, 10.0f, 0);
        (*vertices)[22 + 2 * i] = osg::Vec3(-10.0f, -10.0f + 2*i, 0);
        (*vertices)[22 + 2 * i + 1] = osg::Vec3(10.0f, -10.0f + 2*i, 0);

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
    return new QIcon("data/resources/icons/3D.png");
}