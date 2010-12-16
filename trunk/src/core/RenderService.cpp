#include "CorePCH.h"
#include "RenderService.h"
#include <QtGui/qwidget.h>

#include "C3DModel.h"
#include "Model.h"
#include "Mesh.h"

#include <core/IDataManager.h>
#include <core/SkeletonNode.h>
#include <core/IModel.h>
#include <core/Vec3.h>

#include "Factory.h"

#include <osgWidget/ViewerEventHandlers>
#include <core/MultiViewWidgetItem.h>
#include <core/MultiView.h>
#include <math.h>
#include <osg/Viewport>
#include <stack>
#include <boost/foreach.hpp>
#include <core/MultiViewCameraItem.h>
#include <core/MultiViewManipulatorItem.h>

// TODO: tymczasowo RenderService bêdzie udostêpnia³ instancje Factory
// jednkaze dla zachowania oddzielnosci zadañ trzeba bêdzie usun¹c t¹ mo¿liwoœæ w render servisie

#define pPat osg::PositionAttitudeTransform*
#define MODEL_SIZE 60

// This handles the forwarding of keypress events.
class ShowHideWindowHandler: public osgGA::GUIEventHandler 
{
public:

    ShowHideWindowHandler(osgWidget::Window* window) : window(window) {}

    virtual bool handle(
        const osgGA::GUIEventAdapter& gea,
        osgGA::GUIActionAdapter&      gaa,
        osg::Object*                  obj,
        osg::NodeVisitor*             nv
    ) 
    {
        osgGA::GUIEventAdapter::EventType ev = gea.getEventType();

        if(ev != osgGA::GUIEventAdapter::KEYUP) return false;

        int key     = gea.getKey();
        int keyMask = gea.getModKeyMask();

        // -1 is the "key invalid" return code.
        if(key == 'h' || key == 'H') {
            if ( window->isVisible() ) {
                window->hide();
            } else {
                window->show();
            }
        }
        return false;
    }

protected:
    osg::observer_ptr<osgWidget::Window> window;

};


class MaskableOrbitManipulator : public osgGA::OrbitManipulator
{
    typedef osgGA::OrbitManipulator Base;
private:
    //! Maska przycisków myszy.
    unsigned maskedMouseButtons;

public:
    //! \param flags
    MaskableOrbitManipulator( int flags = DEFAULT_SETTINGS ) :
    Base(flags), maskedMouseButtons(0)
    {}

    //! \param om
    //! \param copyOp
    MaskableOrbitManipulator( const MaskableOrbitManipulator& om, const osg::CopyOp& copyOp = osg::CopyOp::SHALLOW_COPY ) :
    Base(om, copyOp), maskedMouseButtons(om.maskedMouseButtons)
    {
    }

    META_Object( core, MaskableOrbitManipulator );

    //! \return
    unsigned getMaskedMouseButtons() const
    { 
        return maskedMouseButtons;
    }
    //! \param maskedMouseButtons
    void setMaskedMouseButtons(unsigned maskedMouseButtons) 
    { 
        this->maskedMouseButtons = maskedMouseButtons; 
    }

protected:
    virtual bool performMovementLeftMouseButton( const double eventTimeDelta, const double dx, const double dy )
    {
        if ( (maskedMouseButtons & osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON) == 0 ) {
            return Base::performMovementLeftMouseButton(eventTimeDelta, dx, dy);
        } else {
            return false;
        }
    }
    virtual bool performMovementMiddleMouseButton( const double eventTimeDelta, const double dx, const double dy )
    {
        if ( (maskedMouseButtons & osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON) == 0 ) {
            return Base::performMovementMiddleMouseButton(eventTimeDelta, dx, dy);
        } else {
            return false;
        }
    }
    virtual bool performMovementRightMouseButton( const double eventTimeDelta, const double dx, const double dy )
    {
        if ( (maskedMouseButtons & osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON) == 0 ) {
            return Base::performMovementRightMouseButton(eventTimeDelta, dx, dy);
        } else {
            return false;
        }
    }

};


//--------------------------------------------------------------------------------------------------
RenderService::RenderService()
:   name("Render")
{

}

//--------------------------------------------------------------------------------------------------
RenderService::~RenderService()
{
    Clear();
}

//--------------------------------------------------------------------------------------------------
void RenderService::Clear()
{
    widget = NULL;
    m_pModel = NULL;
    SceneRoot = NULL;
}

//--------------------------------------------------------------------------------------------------
AsyncResult RenderService::loadData(IServiceManager* serviceManager, IDataManager* dataManager )
{
    //nowy SetScene
	std::string meshpath = "";
	std::string skelpath = "";
	std::string c3dpath = "";
	if(dataManager->getMeshFilePathCount() > 0)
	{
		meshpath = dataManager->getMeshFilePath(0);
	}
	if(dataManager->getSkeletonFilePathCount() > 0)
	{
		skelpath = dataManager->getSkeletonFilePath(0);
	}
	if(dataManager->getC3dFilePathCount() > 0)
	{
		c3dpath = dataManager->getC3dFilePath(0);
	}

    std::vector<std::string> animationPathList = *dataManager->getAnimationPathList();

	m_pModel = dynamic_cast<Model* >(m_pFactory->GetModel(meshpath, skelpath, animationPathList));

    if(m_pModel) {
        SetScene(m_pModel);
    }
	m_pC3DModel = dynamic_cast<C3DModel* >(m_pFactory->GetC3DModel(c3dpath));

    if(m_pC3DModel) {
        RenderC3D(m_pC3DModel);
    }

    return AsyncResult_Complete;
}

//--------------------------------------------------------------------------------------------------
AsyncResult RenderService::init(IServiceManager* serviceManager, osg::Node* sceneRoot, IDataManager* dataManager)
{
    m_pFactory = new Factor();

    std::cout<< "RenderService ADDED-test!" << std::endl; 
    Inicialize(sceneRoot);
    return AsyncResult_Complete; 
}

//--------------------------------------------------------------------------------------------------
void RenderService::Inicialize(osg::Node* sceneRoot)
{
    SceneRoot = dynamic_cast<osg::Group*>(sceneRoot);
    UTILS_ASSERT(SceneRoot != NULL);

    osgGA::OrbitManipulator *cameraManipulator = new osgGA::OrbitManipulator();

    widget = new QOSGViewer(NULL, 0, 0);
    widget->addEventHandler(new osgViewer::StatsHandler);
    widget->setCameraManipulator(cameraManipulator);
    widget->setMinimumSize(100, 100);

    // stworzenie i dodanie œwiat³a przyczepionego do kamery
    osg::Light* myLight2 = new osg::Light;
    myLight2->setLightNum(1);
    myLight2->setPosition(osg::Vec4(0.0,0.0,0.0,1.0f));
    myLight2->setAmbient(osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
    myLight2->setDiffuse(osg::Vec4(1.0f,1.0f,1.0f,0.0f));
    myLight2->setConstantAttenuation(1.0f);
    myLight2->setLinearAttenuation(1.0f/MODEL_SIZE);
    myLight2->setQuadraticAttenuation(1.0f/osg::square(MODEL_SIZE));

    widget->setLight(myLight2);

    osg::Switch* root = new osg::Switch();
    root->addChild(SceneRoot);


    const osg::GraphicsContext::Traits* traits = widget->getCamera()->getGraphicsContext()->getTraits();

    // tworzymy multi widok
    multiView = new core::MultiView(widget, traits->width, traits->height, 0xF0000000, 0);
    multiView->addChild(new osgWidget::Box("HACK"));

    // stworzenie kamery
    osg::Camera* multiViewCamera = multiView->createParentOrthoCamera();
    multiViewCamera->setClearMask(GL_DEPTH_BUFFER_BIT);
    multiViewCamera->setRenderOrder(osg::Camera::POST_RENDER, 1);
    multiViewCamera->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );


    widget->getCamera()->setClearColor(osg::Vec4(0.73f, 0.73f, 0.73f, 1));

    root->addChild(multiViewCamera);
    widget->setSceneData(root);

    for (int i = 0; i < 4; ++i) {

        osg::Camera* camera = new osg::Camera();
        camera->setName( boost::lexical_cast<std::string>(i) );
        
        MaskableOrbitManipulator* manipulator = new MaskableOrbitManipulator();
        manipulator->setMaskedMouseButtons( osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON );
        manipulator->setHomePosition( 
            osg::Vec3d((i%2) ? 150 - (i/2)*300 : 0 , (i%2) ? 0 : 150 - (i/2)*300, 50), 
            osg::Vec3d(0, 0, 50), 
            osg::Vec3d(0, 0, 1)
            );
        manipulator->home(0);

        addView(camera, manipulator);
    }


    // stworzenie helpa
    std::string lines[] = {
        "Usage:",
        "space - default position",
        "mouse scroll - move camera forward/backward",
        "rmb + mouse move - move camera forward/backward",
        "mmb + mouse move - move camera up/down/left/right",
        "h - show/hide this infoBox"
    };
    osgWidget::Box* infoBox = new osgWidget::Box("infoBox", osgWidget::Box::VERTICAL);
    infoBox->getBackground()->setColor(0.5f, 0.5f, 0.5f, 0.5f);
    multiView->addChild( infoBox );

    osg::ref_ptr<osgWidget::Label> linePrototype = new osgWidget::Label("label", "label");
    linePrototype->setAlignHorizontal( osgWidget::Widget::HA_LEFT );
    linePrototype->setPadLeft( 1 );
    linePrototype->setPadRight( 1 );

    BOOST_REVERSE_FOREACH(std::string& line, lines) {
        osgWidget::Label* label = osg::clone(linePrototype.get(), line, osg::CopyOp::DEEP_COPY_ALL);
        label->setLabel(line);
        infoBox->addWidget(label);
    }

    


    widget->addEventHandler( new ShowHideWindowHandler(infoBox) );
    widget->addEventHandler( new osgWidget::MouseHandler(multiView) );
    widget->addEventHandler( new osgWidget::ResizeHandler(multiView, multiViewCamera) );
    widget->addEventHandler( new osgGA::StateSetManipulator( widget->getCamera()->getOrCreateStateSet() ) );
}

void RenderService::addView( osg::Camera* camera, osgGA::CameraManipulator* manipulator )
{
    UTILS_ASSERT(camera && manipulator);

    // inicjalizacja kamery
    camera->insertChild(0, SceneRoot);
    camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);

    // dodanie kamery do sceny
    widget->getSceneData()->asGroup()->addChild(camera);

    core::MultiViewCameraItem* item = new core::MultiViewCameraItem(camera, widget);
    multiView->addItem(item, new core::MultiViewManipulatorItem(camera, widget, manipulator) );

    camera->setProjectionMatrix( widget->getCamera()->getProjectionMatrix() );
    camera->setViewMatrix( manipulator->getInverseMatrix() );
}

//--------------------------------------------------------------------------------------------------
void RenderService::AddObjectToRender( osg::Node* node )
{
    SceneRoot->addChild(node);
}

//--------------------------------------------------------------------------------------------------
void RenderService::SetScene(osg::Group* root)
{
    Model* model = dynamic_cast<Model*>(root);

    if(model)
        InicizlizeModelMesh(model);
    

    SceneRoot->addChild(root);

    // ustawienie korzenia sceny
    osg::Switch* sceneRoot = widget->getSceneData()->asSwitch();
    sceneRoot->setChild(0, SceneRoot);

    // HACK: trzeba bêdzie wykorzystaæ mechanizm slaves
    for ( unsigned i = 2; i < sceneRoot->getNumChildren(); ++i ) {
        if ( osg::Node* node = sceneRoot->getChild(i) ) {
            if ( osg::Camera* camera = dynamic_cast<osg::Camera*>(node) ) {
                camera->setChild(0, SceneRoot);
            }
        }
    }
}

//--------------------------------------------------------------------------------------------------
bool RenderService::loadShaderSource(osg::Shader* obj, const std::string& fileName )
{
    std::string fqFileName = osgDB::findDataFile(fileName);
    if( fqFileName.length() == 0 )
    {
        std::cout << "File \"" << fileName << "\" not found." << std::endl;
        return false;
    }
    bool success = obj->loadShaderSourceFromFile( fqFileName.c_str());
    if ( !success  )
    {
        std::cout << "Couldn't load file: " << fileName << std::endl;
        return false;
    }
    else
    {
        return true;
    }
}

//--------------------------------------------------------------------------------------------------
osgViewer::Scene* RenderService::GetMainWindowScene()
{
    if(widget)
        return widget->getScene();

    return NULL;
}

//--------------------------------------------------------------------------------------------------
QOSGViewer* RenderService::GetMainAdapterWidget()
{
    if(widget)
        return widget;

    return NULL;
}

//--------------------------------------------------------------------------------------------------
osgGroupPtr RenderService::GetRoot()
{
    return SceneRoot;
}

//--------------------------------------------------------------------------------------------------
void RenderService::TestScreenForNewModel(IModel* model )
{
    UTILS_ASSERT(false, "Nie powinien tutaj wejœæ...");
}

//--------------------------------------------------------------------------------------------------
void RenderService::InicizlizeModelMesh(Model* model)
{
    std::vector<IMesh*>::iterator it;
    std::vector<IMesh*> meshList = model->GetMeshList();

    osg::ref_ptr<osg::Geode> geode = new osg::Geode();

    geode->setName("NEW_MESH");

    for( it = meshList.begin(); it != meshList.end(); it++)
    {
        geode->addDrawable(dynamic_cast<Mesh*>(*it));
    }

    m_pMeshGeode = geode;
    model->addChild(m_pMeshGeode);
}

//--------------------------------------------------------------------------------------------------
// TODO: ten sam przypadek co koœci - poprawic i przekszta³ciæ aby dzia³a³y jak mesh
void RenderService::RenderC3D(IC3DModel* c3dmodel)
{
    osg::ref_ptr<osg::Geode> markersGeode = c3dmodel->GetMarkerGeode();
    markersGeode->setName("markers_geode");

    c3dmodel->DrawMarkers();
    dynamic_cast<C3DModel*>(c3dmodel)->addChild(markersGeode);

    m_pMarkerGeode = markersGeode;

    AddObjectToRender(dynamic_cast<C3DModel*>(c3dmodel));
}

//--------------------------------------------------------------------------------------------------
void RenderService::SetC3DMarkerToRender(IC3DModel *c3dmodel)
{
    C3DModel* tempModel = dynamic_cast<C3DModel*>(c3dmodel);
    m_pMarkerGeode = tempModel->GetMarkerGeode();

    if(!SceneRoot->containsNode(tempModel))
        AddObjectToRender(tempModel);

    DisableMarker();
}

//--------------------------------------------------------------------------------------------------
IWidget* RenderService::getWidget()
{
    return reinterpret_cast<IWidget*>(widget);
}

//--------------------------------------------------------------------------------------------------
osg::Light* RenderService::GetLight()
{
     return widget->getLight();
}

//--------------------------------------------------------------------------------------------------
void RenderService::EnableMesh()
{
    InicizlizeModelMesh(m_pModel);
}

//--------------------------------------------------------------------------------------------------
void RenderService::DisableMesh()
{
    if(m_pMeshGeode.valid() && m_pModel)
        m_pModel->removeChild(m_pMeshGeode.get());
}

//--------------------------------------------------------------------------------------------------
void RenderService::EnableBone()
{
    //RenderBone(m_pModel)
}

//--------------------------------------------------------------------------------------------------
void RenderService::DisableBone()
{
    m_pModel->RemoveGeode();
    SceneRoot;

    if(m_pBoneGeode.valid() && m_pModel)
        m_pModel->removeChild(m_pBoneGeode.get());
}

//--------------------------------------------------------------------------------------------------
void RenderService::EnableMarker()
{
 //   RenderC3D(m_pC3DModel);
}

//--------------------------------------------------------------------------------------------------
void RenderService::DisableMarker()
{
    for(int i = 0; i < SceneRoot->getNumChildren(); i++)
    {
        if(dynamic_cast<C3DModel*>(SceneRoot->getChild(i)))
        {
            dynamic_cast<C3DModel*>(SceneRoot->getChild(i))->RemoveGeode();
        }
    }
}

//--------------------------------------------------------------------------------------------------
IFactor* RenderService::GetFactory()
{
    return m_pFactory;
}

//--------------------------------------------------------------------------------------------------
osg::Node* RenderService::debugGetLocalSceneRoot()
{
    return widget->getSceneData();
}

//--------------------------------------------------------------------------------------------------
void RenderService::CreatingAndRenderMarkerPath()
{
    /*

    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    // create Geometry object to store all the vertices and lines primitive.
    osg::ref_ptr<osg::Geometry>	linesGeom = new osg::Geometry();
    float size = 0.5f; 
    int sizeX = 21; 
    int sizeY = 21; 
    // this time we'll preallocate the vertex array to the size we
    // need and then simple set them as array elements, 8 points
    // makes 4 line segments.
    osg::Vec3Array* vertices = new osg::Vec3Array((sizeX+sizeY)*2);
    for (int i=0; i<sizeX; ++i)
    {
        float a_x = (float(i - sizeX/2) )*size; 
        float a_y = -1.0f*(sizeX/2)*size; 
        float b_x = (float(i - sizeX/2) )*size; 
        float b_y = 1.0f*(sizeX/2)*size; 
        (*vertices)[2*i].set( a_x, a_y, 0.0f);
        (*vertices)[2*i+1].set( b_x, b_y, 0.0f);
    }

    for (int i=0; i<sizeY; ++i)
    {
        float a_x = -1.0f*(sizeY/2)*size; 
        float a_y = (float(i - sizeY/2) )*size; 
        float b_x = 1.0f*(sizeY/2)*size; 
        float b_y = (float(i - sizeY/2) )*size; 
        (*vertices)[sizeX*2+2*i].set( a_x, a_y, 0.0f);
        (*vertices)[sizeX*2+2*i+1].set( b_x, b_y, 0.0f);
    }
    // pass the created vertex array to the points geometry object.
    linesGeom->setVertexArray(vertices);
    // set the colors as before, plus using the above
    osg::Vec4Array* colors = new osg::Vec4Array;
    colors->push_back( osg::Vec4(0.5f, 0.5f, 0.5f, 0.5f) );
    linesGeom->setColorArray(colors);
    linesGeom->setColorBinding( osg::Geometry::BIND_OVERALL );
    // set the normal in the same way color.
    osg::Vec3Array* normals = new osg::Vec3Array;
    normals->push_back( osg::Vec3(0.0f, -1.0f, 0.0f) );
    linesGeom->setNormalArray(normals);
    linesGeom->setNormalBinding( osg::Geometry::BIND_OVERALL );
    // This time we simply use primitive, and hardwire the number of coords to use 
    // since we know up front,
    linesGeom->addPrimitiveSet( new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, (sizeX+sizeY)*2) );
    // add the points geometry to the geode.
    geode->addDrawable(linesGeom);
    //osgView_->setSceneData((osg::Node *)geode);
    geode->setName("grid");
    return geode; 

    */
}