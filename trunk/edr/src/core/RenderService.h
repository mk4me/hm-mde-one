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
#include <osgui/QOsgWidgets.h>
#include <core/IService.h>
#include <core/IFactory.h>

#include <core/MultiView.h>


class C3DModel;
class Model;
class Mesh;
class IModel;
class core::IDataManager;
class Factor;

//--------------------------------------------------------------------------------------------------
// Render Service
//--------------------------------------------------------------------------------------------------

typedef osg::ref_ptr<osg::Group> osgGroupPtr;

class RenderService: public IRenderService
{
    UNIQUE_ID("{A31152B5-B900-4C69-983A-8B2B1F8FF6DB}", "Render Service");
public:
    RenderService();
    virtual ~RenderService();
    virtual void init(core::IServiceManager* serviceManager, core::IDataManager* dataManager); 

    virtual void loadData(core::IServiceManager* serviceManager, core::IDataManager* dataManager);

    void TestScreenForNewModel(IModel* model);
    void AddObjectToRender(osg::Node* node);
    void SetScene(osg::Group* root);
    void Clear();

   
    osgViewer::Scene* GetMainWindowScene();
    osgGroupPtr GetRoot();

    osg::Light* GetLight();

    virtual void RenderC3D(IC3DModel* c3dmodel);

    virtual const std::string& getName() const
    {
        return name;
    }

    virtual QWidget* getWidget();

    virtual void EnableMesh();
    virtual void DisableMesh();

    virtual void EnableBone();
    virtual void DisableBone();

    virtual void EnableMarker();
    virtual void DisableMarker();

    virtual void SetC3DMarkerToRender(IC3DModel *c3dmodel);

    virtual IFactor* GetFactory();

    //! \ Creating and render Marker Path
    virtual void CreatingAndRenderMarkerPath(IC3DModel* c3dModel, std::vector<int > markerSelectedList);

private: 
    void DisablemarkerPath();

    void Inicialize();
    void InicizlizeModelMesh(Model* model);
    void AddShaders(osg::Group* group);

    osg::ref_ptr<osg::Geode> m_pMeshGeode;
    osg::ref_ptr<osg::Geode> m_pBoneGeode;
    osg::ref_ptr<osg::Geode> m_pMarkerGeode;
    osg::ref_ptr<osg::Geode> m_pMarkerLineGeode;

    osgui::QOsgDefaultWidget* widget; 
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

typedef core::shared_ptr<RenderService> RenderServicePtr;
typedef core::shared_ptr<const RenderService> RenderServiceConstPtr;
typedef core::weak_ptr<RenderService> RenderServiceWeakPtr;

#endif //RENDER_SERVICE_H