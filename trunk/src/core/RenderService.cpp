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
void RenderService::TestScreenForNewModel( Model* model )
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
}

IWidget* RenderService::getWidget()
{
    return reinterpret_cast<IWidget*>(widget);
}