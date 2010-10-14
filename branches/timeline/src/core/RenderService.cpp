#include "CorePCH.h"
#include "RenderService.h"
#include <QtGui/qwidget.h>

#include "UserInterfaceService.h"
#include "Model.h"
#include "Mesh.h"

#include <core/IModel.h>
#include <core/IDataManager.h>

#include <osgGA/TerrainManipulator>
#include <core/SkeletonNode.h>

#define pPat osg::PositionAttitudeTransform*

M_DECLARED_CLASS(RenderService, kCLASSID_RenderService);

//--------------------------------------------------------------------------------------------------
RenderService::RenderService()
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
    m_pMainOsgView = NULL;
    m_spRoot = NULL;
    m_pUserInterfaceService = NULL;

    m_widgetMap.clear();
}

//--------------------------------------------------------------------------------------------------
AsyncResult RenderService::OnAdded(IServiceManager* serviceManager)
{
    m_pServiceManager = (ServiceManager*)serviceManager;

    std::cout<< "RenderService ADDED-test!" << std::endl; 
    Inicialize();
    return AsyncResult_Complete; 
}

//--------------------------------------------------------------------------------------------------
void RenderService::Inicialize()
{
    m_pUserInterfaceService =  dynamic_cast<UserInterfaceService*>(m_pServiceManager->GetSystemService(UserInterfaceService::CLASS_ID));

    osgGA::TerrainManipulator *cameraManipulator = new osgGA::TerrainManipulator();
    QMainWindow* mainWindow = (QMainWindow*)m_pUserInterfaceService->GetMainObject();
    QWidget* widget = new QWidget();  
    QDockWidget *dock; 

    dock = new QDockWidget("RENDERING_MAIN_WINDOW", mainWindow, Qt::WindowTitleHint);
    dock->setObjectName("RENDERING_DOC");
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    mainWindow->addDockWidget(Qt::NoDockWidgetArea, dock);
    m_widgetMap["RENDERING_MAIN_WINDOW"] = widget;


    
    m_spRoot = new osg::Group();
    m_spRoot->setName("root");

    m_pMainOsgView = new ViewerQT(m_pServiceManager, widget, 0, 0);
    m_pMainOsgView->addEventHandler(new osgViewer::StatsHandler);
    m_pMainOsgView->setCameraManipulator(cameraManipulator);

    m_pMainOsgView->setSceneData(m_spRoot);
    dock->setWidget((QWidget*)m_pMainOsgView); 
    mainWindow->setCentralWidget(dock);
}

//--------------------------------------------------------------------------------------------------
void RenderService::AddObjectToRender( osg::Node* node )
{
    m_spRoot->addChild(node);
}

//--------------------------------------------------------------------------------------------------
void RenderService::SetScene(osg::Group* root)
{
    Model* model = dynamic_cast<Model*>(root);
    if(model)
    {
        InicizlizeModelMesh(model);
    }

    m_spRoot = root;
    m_pMainOsgView->setSceneData(m_spRoot);
    
    ViewerQT* temp;
    std::map<std::string, ViewerQT*>::iterator it;

    for (it = m_osgViewMap.begin(); it != m_osgViewMap.end(); it++) 
    {
        temp = (*it).second;
        temp->setSceneData(root);
    }
}

//--------------------------------------------------------------------------------------------------
osgViewer::Scene* RenderService::GetMainWindowScene()
{
    if(m_pMainOsgView)
        return m_pMainOsgView->getScene();

    return NULL;
}

//--------------------------------------------------------------------------------------------------
ViewerQT* RenderService::GetMainAdapterWidget()
{
    if(m_pMainOsgView)
        return m_pMainOsgView;

    return NULL;
}

//--------------------------------------------------------------------------------------------------
osgGroupPtr RenderService::GetRoot()
{
    return m_spRoot;
}

//--------------------------------------------------------------------------------------------------
bool RenderService::CreateNewWindow( std::string windowName )
{
    QMainWindow* mainWindow = (QMainWindow*)m_pUserInterfaceService->GetMainObject();
    
    QDockWidget *dock = new QDockWidget(windowName.c_str(), mainWindow, Qt::WindowTitleHint);
    osgGA::TerrainManipulator *cameraManipulator = new osgGA::TerrainManipulator();
    std::string docWindowName = windowName + "DOC";
    QWidget* widget = new QWidget(); 
    ViewerQT* OsgView = new ViewerQT(m_pServiceManager, widget, 0, 0);

    dock->setObjectName(docWindowName.c_str());
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    mainWindow->addDockWidget(Qt::BottomDockWidgetArea, dock);
    m_widgetMap[windowName] = widget;

    m_spRoot = new osg::Group();
    m_spRoot->setName("root");

    OsgView->addEventHandler(new osgViewer::StatsHandler);
    OsgView->setCameraManipulator(cameraManipulator);

    m_pMainOsgView->setSceneData(m_spRoot);
    dock->setWidget((QWidget*)OsgView); 
    m_osgViewMap[windowName] = OsgView;

    return true;
}

//--------------------------------------------------------------------------------------------------
void RenderService::TestScreenForNewModel( Model* model )
{
    InicizlizeModelMesh(model);
	QMainWindow* mainWindow = (QMainWindow*)m_pUserInterfaceService->GetMainObject();

	QDockWidget *dock = new QDockWidget("TESTING_NEW_MODEL", mainWindow, Qt::WindowTitleHint);
    osgGA::TerrainManipulator *cameraManipulator = new osgGA::TerrainManipulator();
	std::string docWindowName = "TESTING_NEW_MODEL_DOC";
    QWidget* widget = new QWidget();  
    ViewerQT* OsgView = new ViewerQT(m_pServiceManager, widget, 0, 0);

	dock->setObjectName(docWindowName.c_str());
	dock->setAllowedAreas(Qt::AllDockWidgetAreas);
	mainWindow->addDockWidget(Qt::BottomDockWidgetArea, dock);
	m_widgetMap["TESTING_NEW_MODEL"] = widget;

	OsgView->addEventHandler(new osgViewer::StatsHandler);
	OsgView->setCameraManipulator(cameraManipulator);

	OsgView->setSceneData(model);
	dock->setWidget((QWidget*)OsgView); 
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


    // Drawing Bones
    osg::ref_ptr<osg::Geode> skeletonGeode = new osg::Geode();
    skeletonGeode->setName("skeleton_geode");

    int childNum = model->getNumChildren();
    for (int i = 0; i < childNum; ++i)
    {		
        osg::Group* group = dynamic_cast<SkeletonNode*>(model->getChild(i));
        if (group)
        {
            //////////////////////////////////////////////////////////////////////////
            // get skeleton and set pointer at him as member
            osg::ref_ptr<osg::Group> skeleton = model->getChild(i)->asGroup();

            pPat root_bone = (pPat)(skeleton.get());

            for (unsigned int b = 0; b < root_bone->getNumChildren(); ++b)
                if (dynamic_cast<SkeletonNode*>(root_bone->getChild(b)))
                    model->DrawBone((pPat)root_bone->getChild(b), &root_bone->getPosition(), &root_bone->getAttitude(), skeletonGeode);

            model->addChild(skeletonGeode);
        }
    }

    model->addChild(geode);
}

//--------------------------------------------------------------------------------------------------
void RenderService::SetModel(IDataManager* dataManager )
{
    if(dynamic_cast<Model* >(dataManager->GetModel()))
        SetScene(dynamic_cast<Model* >(dataManager->GetModel()));
}
