#include "CorePCH.h"
#include "RenderService.h"
#include <QtGui/qwidget.h>

#include "Model.h"
#include "Mesh.h"

#include <core/IModel.h>
#include <core/IDataManager.h>

#include <osgGA/TerrainManipulator>
#include <core/SkeletonNode.h>

#define pPat osg::PositionAttitudeTransform*

//deprecated:
//M_DECLARED_CLASS(RenderService, kCLASSID_RenderService);

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
    sceneRoot = NULL;
}

//--------------------------------------------------------------------------------------------------
AsyncResult RenderService::init(IServiceManager* serviceManager, osg::Node* sceneRoot)
{
    std::cout<< "RenderService ADDED-test!" << std::endl; 
    Inicialize(sceneRoot);
    return AsyncResult_Complete; 
}

//--------------------------------------------------------------------------------------------------
void RenderService::Inicialize(osg::Node* sceneRoot)
{
    sceneRoot = dynamic_cast<osg::Group*>(sceneRoot);
    UTILS_ASSERT(sceneRoot != NULL);

//     osgGA::TerrainManipulator *cameraManipulator = new osgGA::TerrainManipulator();
//     QMainWindow* mainWindow = (QMainWindow*)m_pUserInterfaceService->GetMainObject();
//     QWidget* widget = new QWidget();  
//     QDockWidget *dock; 
// 
//     dock = new QDockWidget("RENDERING_MAIN_WINDOW", mainWindow, Qt::WindowTitleHint);
//     dock->setObjectName("RENDERING_DOC");
//     dock->setAllowedAreas(Qt::AllDockWidgetAreas);
//     mainWindow->addDockWidget(Qt::NoDockWidgetArea, dock);
//     m_widgetMap["RENDERING_MAIN_WINDOW"] = widget;

    //QWidget* parent = new QWidget();  

    osgGA::TerrainManipulator *cameraManipulator = new osgGA::TerrainManipulator();

    widget = new QOSGViewer(NULL, 0, 0);
    widget->addEventHandler(new osgViewer::StatsHandler);
    widget->setCameraManipulator(cameraManipulator);
    widget->setSceneData(sceneRoot);
}

//--------------------------------------------------------------------------------------------------
void RenderService::AddObjectToRender( osg::Node* node )
{
    sceneRoot->addChild(node);
}

//--------------------------------------------------------------------------------------------------
void RenderService::SetScene(osg::Group* root)
{
    Model* model = dynamic_cast<Model*>(root);
    if(model)
    {
        InicizlizeModelMesh(model);
    }

    sceneRoot = root;
    widget->setSceneData(sceneRoot);
    
//     ViewerQT* temp;
//     std::map<std::string, ViewerQT*>::iterator it;
// 
//     for (it = m_osgViewMap.begin(); it != m_osgViewMap.end(); it++) 
//     {
//         temp = (*it).second;
//         temp->setSceneData(root);
//     }
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
    return sceneRoot;
}

//--------------------------------------------------------------------------------------------------

// TODO:
// R.Zowal: to by³a testowa metoda wykorzystywana wy³¹cznie w setSetting(testowa akcja) - teraz nie aktualna. 
// Metoda mia³a tylko na celu sprawdzenie poprawnoœci generowanie wielu okienek QOSGViewer, równiez tych z plugina
// Sugeruje aby to jeszcze chwilowo zosta³o. (testy wyœwietlania modelu ca³oœciowo w jednym oknie, i modelu szkieletu w kolejnym + ewntualne opcje)
// nied³ugo nie bêdzie potrzebne wtedy to posprz¹tam. 

// Piotr Gwiazdowski: RenderService towrzy okienka? This is madness
// bool RenderService::CreateNewWindow( std::string windowName )
// {
//     QMainWindow* mainWindow = (QMainWindow*)m_pUserInterfaceService->GetMainObject();
//     
//     QDockWidget *dock = new QDockWidget(windowName.c_str(), mainWindow, Qt::WindowTitleHint);
//     osgGA::TerrainManipulator *cameraManipulator = new osgGA::TerrainManipulator();
//     std::string docWindowName = windowName + "DOC";
//     QWidget* widget = new QWidget(); 
//     ViewerQT* OsgView = new ViewerQT(m_pServiceManager, widget, 0, 0);
// 
//     dock->setObjectName(docWindowName.c_str());
//     dock->setAllowedAreas(Qt::AllDockWidgetAreas);
//     mainWindow->addDockWidget(Qt::BottomDockWidgetArea, dock);
//     m_widgetMap[windowName] = widget;
// 
//     sceneRoot = new osg::Group();
//     sceneRoot->setName("root");
// 
//     OsgView->addEventHandler(new osgViewer::StatsHandler);
//     OsgView->setCameraManipulator(cameraManipulator);
// 
//     widget->setSceneData(sceneRoot);
//     dock->setWidget((QWidget*)OsgView); 
//     m_osgViewMap[windowName] = OsgView;
// 
//     return true;
// }

//--------------------------------------------------------------------------------------------------
void RenderService::TestScreenForNewModel( Model* model )
{
    UTILS_ASSERT(false, "Nie powinien tutaj wejœæ...");
//     InicizlizeModelMesh(model);
// 	QMainWindow* mainWindow = (QMainWindow*)m_pUserInterfaceService->GetMainObject();
// 
// 	QDockWidget *dock = new QDockWidget("TESTING_NEW_MODEL", mainWindow, Qt::WindowTitleHint);
//     osgGA::TerrainManipulator *cameraManipulator = new osgGA::TerrainManipulator();
// 	std::string docWindowName = "TESTING_NEW_MODEL_DOC";
//     QWidget* widget = new QWidget();  
//     ViewerQT* OsgView = new ViewerQT(m_pServiceManager, widget, 0, 0);
// 
// 	dock->setObjectName(docWindowName.c_str());
// 	dock->setAllowedAreas(Qt::AllDockWidgetAreas);
// 	mainWindow->addDockWidget(Qt::BottomDockWidgetArea, dock);
// 
// 	OsgView->addEventHandler(new osgViewer::StatsHandler);
// 	OsgView->setCameraManipulator(cameraManipulator);
// 
// 	OsgView->setSceneData(model);
// 	dock->setWidget((QWidget*)OsgView); 
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


    model->addChild(geode);

	RenderBone(model);
}

//--------------------------------------------------------------------------------------------------
AsyncResult RenderService::loadData(IServiceManager* serviceManager, IDataManager* dataManager )
{
    if(dynamic_cast<Model* >(dataManager->GetModel())) {
        SetScene(dynamic_cast<Model* >(dataManager->GetModel()));
    }
    return AsyncResult_Complete;
}

//--------------------------------------------------------------------------------------------------
void RenderService::RenderBone(Model* model)
{
	osg::ref_ptr<osg::Geode> skeletonGeode = model->GetSkeletonGeode();
	skeletonGeode->setName("skeleton_geode");

    if(!model->GetSkeleton())
        return;

	Bone* bone = model->GetSkeleton()->m_pRootBone;
	int childcount = bone->child.size();

	for (int i = 0; i<childcount; i++)
	{
		model->DrawBone(bone->child[i], skeletonGeode);
	}

	model->addChild(skeletonGeode);


// 	// Drawing Bones
// 	osg::ref_ptr<osg::Geode> skeletonGeode = new osg::Geode();
// 	skeletonGeode->setName("skeleton_geode");
// 
// 	int childNum = model->getNumChildren();
// 	for (int i = 0; i < childNum; ++i)
// 	{		
// 		osg::Group* group = dynamic_cast<SkeletonNode*>(model->getChild(i));
// 		if (group)
// 		{
// 			//////////////////////////////////////////////////////////////////////////
// 			// get skeleton and set pointer at him as member
// 			osg::ref_ptr<osg::Group> skeleton = model->getChild(i)->asGroup();
// 
// 			pPat root_bone = (pPat)(skeleton.get());
// 
// 			for (unsigned int b = 0; b < root_bone->getNumChildren(); ++b)
// 				if (dynamic_cast<SkeletonNode*>(root_bone->getChild(b)))
// 					model->DrawBone((pPat)root_bone->getChild(b), &root_bone->getPosition(), &root_bone->getAttitude(), skeletonGeode);
// 
// 			model->addChild(skeletonGeode);
// 		}
// 	}
}

IWidget* RenderService::getWidget()
{
    return reinterpret_cast<IWidget*>(widget);
}