#ifndef RENDER_SERVICE_H
#define RENDER_SERVICE_H

#include <iostream>
#include <vector>

#include <core/SimpleFunctors.h>
#include <osg//Node>
#include <osgViewer/GraphicsWindow>
#include <osgViewer/View>

#include <core/IService.h>
#include <core/QOSGWidget.h>
#include <core/IRenderService.h>

#include <map>

class C3DModel;
class Model;
class Mesh;
class IModel;
class IDataManager;

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
    virtual AsyncResult init(IServiceManager* serviceManager, osg::Node* sceneRoot); 

    virtual AsyncResult loadData(IServiceManager* serviceManager, IDataManager* dataManager);

    void TestScreenForNewModel(IModel* model);
    void AddObjectToRender(osg::Node* node);
    void SetScene(osg::Group* root);
    void Clear();

   
    osgViewer::Scene* GetMainWindowScene();
    osgGroupPtr GetRoot();
    QOSGViewer* GetMainAdapterWidget();

    osg::Light* GetLight();

	virtual void RenderBone(IModel* model);
    virtual void RenderC3D(IC3DModel* c3dmodel);

    void DrawNormals(IModel* model, float size);

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

private: 
    void Inicialize(osg::Node* sceneRoot);
    void InicizlizeModelMesh(Model* model);

    osg::ref_ptr<osg::Geode> m_pMeshGeode;
    osg::ref_ptr<osg::Geode> m_pBoneGeode;
    osg::ref_ptr<osg::Geode> m_pMarkerGeode;

    QOSGViewer* widget; 
    osgGroupPtr sceneRoot;
    //! Nazwa.
    std::string name;
    Model *m_pModel;
    C3DModel *m_pC3DModel;
};

typedef CORE_SHARED_PTR(RenderService) RenderServicePtr;
typedef CORE_CONST_SHARED_PTR(RenderService) RenderServiceConstPtr;
typedef CORE_WEAK_PTR(RenderService) RenderServiceWeakPtr;

#endif //RENDER_SERVICE_H