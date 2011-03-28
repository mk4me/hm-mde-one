#include "PCH.h"

#include <boost/foreach.hpp>
#include <osg/Geode>
#include <core/IServiceManager.h>
#include <plugins/timeline/ITimeline.h>
#include <core/IModel.h>
#include <core/IDataManager.h>
#include <core/IVisualizer.h>
#include "mainwindow.h"
#include "KinematicService.h"
#include <core/Log.h>
#include <core/IService.h>
#include <core/ObjectWrapper.h>
#include <QtCore/QtCore>
#include <QtGui/QtGui>
#include <QtOpenGL/QtOpenGL>
#include <QtCore/QDir>
#include <plugins/kinematic/Wrappers.h>
#include "ISchemeDrawer.h"
#include "OsgSchemeDrawer.h"
#include "LineSchemeDrawer.h"

using namespace core;


KinematicService::KinematicService() :
    name("KinematicService"),
    logic(new SkeletonViewerLogic)
{
   widget = new MainWindow();
   // viz.createWidget();
   SkeletalVisualizationSchemePtr visualization = SkeletalVisualizationScheme::create();
   logic->setVisualization(visualization);
   widget->setLogic(logic);
}

AsyncResult KinematicService::loadData(IServiceManager* serviceManager, core::IDataManager* dataManager)
{
    std::vector<kinematic::KinematicModelPtr> models = core::queryDataPtr(dataManager);
    if (models.size() > 0 && models[0]) {
        logic->setKinematic(models[0]);
    } else {
        return AsyncResult_Failure;
    }
    return AsyncResult_Complete;
}

AsyncResult KinematicService::update( double time, double timeDelta )
{
    return AsyncResult_Complete;
}


void KinematicVisualizer::update( double deltaTime )
{
}

void KinematicVisualizer::setUp( IObjectSource* source )
{
    typedef boost::shared_ptr<const kinematic::KinematicModel> KinematicModelConstPtr;
     KinematicModelConstPtr model = source->getObject(0);
     if (model) {
         scheme = SkeletalVisualizationScheme::create();
         
         scheme->setKinematicModel(model);
         //scheme->setCurrentTime(0.1);
         OsgSchemeDrawerPtr drawer(new LineSchemeDrawer);
         scheme->setSchemeDrawer(drawer);
         rootNode->addChild(drawer->getNode());
         scheme->setCurrentTime(0.0);
     }
}

IVisualizer* KinematicVisualizer::create() const
{
    return new KinematicVisualizer();
}

void KinematicVisualizer::getSlotInfo( int source, std::string& name, ObjectWrapper::Types& types )
{
    if ( source == 0 ) 
    {
        name = "model"; 
        types.push_back(typeid(kinematic::KinematicModel));
    }
}

QWidget* KinematicVisualizer::createWidget()
{
    
    widget = new osgui::QOsgDefaultWidget();
    const osg::GraphicsContext::Traits* traits = widget->getCamera()->getGraphicsContext()->getTraits();
    widget->setTimerActive(true);
    
    // tworzenie kamery
    widget->getCamera()->setClearColor(osg::Vec4(0.705f, 0.72f, 0.705f, 1));
    widget->getCamera()->setNearFarRatio(0.000000001);
    //osg::Vec3 pos( 0.0f, 0.0f, 9.0f);
    //osg::Vec3 up(0, 1, 0);
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

void KinematicVisualizer::updateAnimation()
{
    if (scheme) {
        double interval = widget->getTimerInterval() / 1000.0;
        double duration = scheme->getDuration();
        double time = scheme->getCurrentTime() * duration;
        time += interval;
        if (time >= duration) {
            time = 0.0;
        } else {
            time = time / duration;
        }
        scheme->setCurrentTime(time);
    }
}

GeodePtr KinematicVisualizer::createFloor()
{
    GeodePtr geode = new osg::Geode();
    // todo sparametryzowac
    osg::ref_ptr<osg::Geometry> linesGeom = new osg::Geometry();

    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(44);

    for (int i = 0; i < 11; i++) {
        (*vertices)[2 * i] = osg::Vec3(-5.0f + i, -5.0f, 0);
        (*vertices)[2 * i + 1] = osg::Vec3(-5.0f + i, 5.0f, 0);
        (*vertices)[20 + 2 * i] = osg::Vec3(-5.0f, -5.0f + i, 0);
        (*vertices)[20 + 2 * i + 1] = osg::Vec3(5.0f, -5.0f + i, 0);

    }

    linesGeom->setVertexArray(vertices);

    // todo color
    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
    colors->push_back(osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f));
    linesGeom->setColorArray(colors);
    linesGeom->setColorBinding(osg::Geometry::BIND_OVERALL);


    osg::Vec3Array* normals = new osg::Vec3Array;
    normals->push_back(osg::Vec3(1.0f, -0.0f, 0.0f));
    linesGeom->setNormalArray(normals);
    linesGeom->setNormalBinding(osg::Geometry::BIND_OVERALL);

    linesGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, 44));

    geode->addDrawable(linesGeom);
    return geode;
}