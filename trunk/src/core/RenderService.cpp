#include "CorePCH.h"
#include "RenderService.h"
#include <QtGui/qwidget.h>

#include "Model.h"
#include "Mesh.h"

#include <core/IModel.h>
#include <core/IDataManager.h>

#include <osgGA/TerrainManipulator>
#include <core/SkeletonNode.h>

#include <core/Vec3.h>
#include <osg/Geometry>


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

    osg::Light *light = new osg::Light();

    light->setPosition(osg::Vec4d(300, -200, 900, 40));
    widget->setLight(light);
    
    widget->setLightingMode(osg::View::LightingMode::SKY_LIGHT);

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

//    DrawNormals(model, 2.0f);
}

//--------------------------------------------------------------------------------------------------
void RenderService::DrawNormals(Model* model, float size)
{
    std::vector<IMesh*> meshList = model->GetMeshList();


    osg::ref_ptr<osg::Geode> m_spNormalGeode = model->GetSkeletonGeode();
    m_spNormalGeode->setName("Normals_Vertex");

    for(std::vector<IMesh*>::iterator it = meshList.begin(); it != meshList.end(); it++)
    {
        IMesh* mesh = *it;


        // for every vertice
        for (int v = 0; v < mesh->GetVertexCount(); ++v)
        {
            osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();

            // vertices
            osg::Vec3Array* vertices = new osg::Vec3Array();
            vertices->push_back(osg::Vec3(mesh->GetVerts()[v]._v[0], mesh->GetVerts()[v]._v[1], mesh->GetVerts()[v]._v[2]));
            vertices->push_back(osg::Vec3(mesh->GetVerts()[v]._v[0] + size * mesh->GetVertNormals()[v]._v[0], 
                mesh->GetVerts()[v]._v[1] + size * mesh->GetVertNormals()[v]._v[1], 
                mesh->GetVerts()[v]._v[2] + size * mesh->GetVertNormals()[v]._v[2]));

            // indices
            osg::DrawElementsUInt* line = new osg::DrawElementsUInt(osg::PrimitiveSet::LINES, 0);
            line->push_back(0);
            line->push_back(1);

            // set geometry data
            geometry->setVertexArray(vertices);
            geometry->addPrimitiveSet(line);

            // set colors
            osg::Vec4Array* colors = new osg::Vec4Array;
            colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
            colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));

            osg::TemplateIndexArray<unsigned int, osg::Array::UIntArrayType,4,4> *colorIndexArray;
            colorIndexArray = new osg::TemplateIndexArray<unsigned int, osg::Array::UIntArrayType,4,4>;
            colorIndexArray->push_back(0);
            colorIndexArray->push_back(1);

            geometry->setColorArray(colors);
            geometry->setColorIndices(colorIndexArray);
            geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

            m_spNormalGeode->addDrawable(geometry);
        }
    }

    model->addChild(m_spNormalGeode);
};

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
