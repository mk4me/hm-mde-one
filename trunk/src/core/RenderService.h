/********************************************************************
	created:  
	created:  
	filename: FileReader2Modion.h
	author:	  Rafal Zowal
	
    purpose: Us³uga  Zarz¹dzanie Renderingiem

*********************************************************************/

#ifndef RENDER_SERVICE_H
#define RENDER_SERVICE_H

#include <iostream>
#include <vector>
#include <string>
#include <map>

#include <osgGA/TerrainManipulator>
#include <osgViewer/GraphicsWindow>
#include <osg/MatrixTransform>
#include <osg/StateAttribute>
#include <osg/LightSource>
#include <osgViewer/View>
#include <osg/Geometry>
#include <osg//Node>
#include <osg/Light>
#include <osg/Point>

#include <osgDB/FileUtils>

#include <osg/Shader>
#include <osg/Program>
#include <osg/VertexProgram>
#include <osg/FragmentProgram>

#include <core/SimpleFunctors.h>
#include <core/IRenderService.h>
#include <core/QOSGWidget.h>
#include <core/IService.h>
#include <core/IFactory.h>

#include <core/MultiView.h>


class C3DModel;
class Model;
class Mesh;
class IModel;
class IDataManager;
class Factor;

//--------------------------------------------------------------------------------------------------
// Render Service
//--------------------------------------------------------------------------------------------------

typedef osg::ref_ptr<osg::Group> osgGroupPtr;

class RenderService: public IRenderService
{
    UNIQUE_ID('REND','SRVC');
public:
    RenderService();
    virtual ~RenderService();
    virtual AsyncResult init(IServiceManager* serviceManager, osg::Node* sceneRoot, IDataManager* dataManager); 

    virtual AsyncResult loadData(IServiceManager* serviceManager, IDataManager* dataManager);

    void TestScreenForNewModel(IModel* model);
    void AddObjectToRender(osg::Node* node);
    void SetScene(osg::Group* root);
    void Clear();

   
    osgViewer::Scene* GetMainWindowScene();
    osgGroupPtr GetRoot();
    QOSGViewer* GetMainAdapterWidget();

    osg::Light* GetLight();

    virtual void RenderC3D(IC3DModel* c3dmodel);

    virtual const std::string& getName() const
    {
        return name;
    }

    virtual IWidget* getWidget();

    virtual void EnableMesh();
    virtual void DisableMesh();

    virtual void EnableBone();
    virtual void DisableBone();

    virtual void EnableMarker();
    virtual void DisableMarker();

    virtual void SetC3DMarkerToRender(IC3DModel *c3dmodel);

    virtual IFactor* GetFactory();

private: 

    void Inicialize(osg::Node* sceneRoot);
    void InicizlizeModelMesh(Model* model);
    bool loadShaderSource(osg::Shader* obj, const std::string& fileName);
    void AddShaders(osg::Group* group);

    osg::ref_ptr<osg::Geode> m_pMeshGeode;
    osg::ref_ptr<osg::Geode> m_pBoneGeode;
    osg::ref_ptr<osg::Geode> m_pMarkerGeode;

    QOSGViewer* widget; 
    osgGroupPtr SceneRoot;
    //! Nazwa.
    std::string name;
    Model *m_pModel;
    C3DModel *m_pC3DModel;

    Factor* m_pFactory;

    osg::ref_ptr<core::MultiView> multiView;

public:
    //! \param name
    //! \param camera
    //! \param manipulator
    virtual void addView(osg::Camera* camera, osgGA::CameraManipulator* manipulator);

    //!
    virtual osg::Node* debugGetLocalSceneRoot();
};

typedef CORE_SHARED_PTR(RenderService) RenderServicePtr;
typedef CORE_CONST_SHARED_PTR(RenderService) RenderServiceConstPtr;
typedef CORE_WEAK_PTR(RenderService) RenderServiceWeakPtr;

#endif //RENDER_SERVICE_H