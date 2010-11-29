#include "CorePCH.h"
#include "RenderService.h"
#include <QtGui/qwidget.h>

#include "C3DModel.h"
#include "Model.h"
#include "Mesh.h"

#include <core/IModel.h>
#include <core/IDataManager.h>

#include <osgGA/TerrainManipulator>
#include <core/SkeletonNode.h>

#include <core/Vec3.h>
#include <osg/Geometry>

#include "Factory.h"


// TODO: tymczasowo RenderService b�dzie udost�pnia� instancje Factory
// jednkaze dla zachowania oddzielnosci zada� trzeba b�dzie usun�c t� mo�liwo�� w render servisie

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
    m_pModel = NULL;
    sceneRoot = NULL;
}

//--------------------------------------------------------------------------------------------------
AsyncResult RenderService::loadData(IServiceManager* serviceManager, IDataManager* dataManager )
{
//    m_pFactory = new Factor();

    //nowy SetScene
    m_pModel = dynamic_cast<Model* >(m_pFactory->GetModel(dataManager->GetMeshFilePathPath(0), dataManager->GetSkeletonFilePath(0), *dataManager->GetAnimationList()));

    if(m_pModel) {
        SetScene(m_pModel);
    }

    m_pC3DModel = dynamic_cast<C3DModel* >(m_pFactory->GetC3DModel(dataManager->GetC3dFilePath(0)));

    if(m_pC3DModel) {
        RenderC3D(m_pC3DModel);
    }


//     if(dynamic_cast<Model* >(dataManager->GetModel())) {
//         SetScene(dynamic_cast<Model* >(dataManager->GetModel()));
//         m_pModel = dynamic_cast<Model* >(dataManager->GetModel());
//     }


//     if(dynamic_cast<C3DModel* >(dataManager->GetC3DModel())) {
//         RenderC3D(dynamic_cast<C3DModel* >(dataManager->GetC3DModel()));
//         m_pC3DModel = dynamic_cast<C3DModel* >(dataManager->GetC3DModel());
//     }

    return AsyncResult_Complete;
}

//--------------------------------------------------------------------------------------------------
AsyncResult RenderService::init(IServiceManager* serviceManager, osg::Node* sceneRoot)
{
    m_pFactory = new Factor();

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

    light->setPosition(osg::Vec4d(10, -10, 10, 4));
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
        InicizlizeModelMesh(model);
    
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
void RenderService::TestScreenForNewModel(IModel* model )
{
    UTILS_ASSERT(false, "Nie powinien tutaj wej��...");
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
// TODO: kosci trzeba zamienic jako geometry i aktualizowa� ich pozycje - tak jak mesh
// Tymczasowo nie uzywana metoda - update  kosci robi sie poprzez model
// this method is uset to display Skeleton from asf - only 1 time TODO: Change it
void RenderService::RenderBone(IModel* model)
{
	m_pBoneGeode = model->GetSkeletonGeode().get();
	m_pBoneGeode->setName("skeleton_geode");

    if(!model->GetSkeleton())
        return;

	Bone* bone = model->GetSkeleton()->m_pRootBone;
	int childcount = bone->child.size();

	for (int i = 0; i<childcount; i++)
	{
		dynamic_cast<Model*>(model)->DrawBone(bone->child[i], m_pBoneGeode);
	}

	dynamic_cast<Model*>(model)->addChild(m_pBoneGeode);
}

//--------------------------------------------------------------------------------------------------
// TODO: ten sam przypadek co ko�ci - poprawic i przekszta�ci� aby dzia�a�y jak mesh
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
    DisableMarker();

    C3DModel* tempModel = dynamic_cast<C3DModel*>(c3dmodel);
    m_pMarkerGeode = tempModel->GetMarkerGeode();

    if(!sceneRoot->containsNode(tempModel))
        AddObjectToRender(tempModel);
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
    for(int i = 0; i < sceneRoot->getNumChildren(); i++)
    {
        if(dynamic_cast<C3DModel*>(sceneRoot->getChild(i)))
        {
            dynamic_cast<C3DModel*>(sceneRoot->getChild(i))->RemoveGeode();
        }
    }
}

//--------------------------------------------------------------------------------------------------
IFactor* RenderService::GetFactory()
{
    return m_pFactory;
}

//--------------------------------------------------------------------------------------------------
// Testowa metoda
void RenderService::DrawNormals(IModel* model, float size)
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

    dynamic_cast<Model*>(model)->addChild(m_spNormalGeode);
};