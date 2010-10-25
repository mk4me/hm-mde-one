#ifndef RENDER_SERVICE_H
#define RENDER_SERVICE_H

#include <iostream>
#include <vector>

#include <core/GlobalServicesIDs.h>
#include <core/SimpleFunctors.h>
#include <osg//Node>
#include <osgViewer/GraphicsWindow>
#include <osgViewer/View>

#include <core/QOSGWidget.h>

#include "AdapterWidget.h"

#include <map>

class Model;
class Mesh;
class IModel;
class IDataManager;

//--------------------------------------------------------------------------------------------------
// Render Service
//--------------------------------------------------------------------------------------------------

typedef osg::ref_ptr<osg::Group> osgGroupPtr;

class RenderService: public IService
{
    UNIQUE_ID('REND','SRVC');
public:
    RenderService();
    virtual ~RenderService();
    virtual AsyncResult init(IServiceManager* serviceManager, osg::Node* sceneRoot); 

    virtual AsyncResult loadData(IServiceManager* serviceManager, IDataManager* dataManager);

    void TestScreenForNewModel(Model* model);
    void AddObjectToRender(osg::Node* node);
    void SetScene(osg::Group* root);
    void Clear();

    

    osgViewer::Scene* GetMainWindowScene();
    osgGroupPtr GetRoot();
    QOSGViewer* GetMainAdapterWidget();

	void RenderBone(Model* model);

    virtual const std::string& getName() const
    {
        return name;
    }
    virtual IWidget* getWidget();

private: 
    void Inicialize(osg::Node* sceneRoot);
    void InicizlizeModelMesh(Model* model);

    QOSGViewer* widget; 
    osgGroupPtr sceneRoot;
    //! Nazwa.
    std::string name;
};

typedef CORE_SHARED_PTR(RenderService) RenderServicePtr;
typedef CORE_CONST_SHARED_PTR(RenderService) RenderServiceConstPtr;
typedef CORE_WEAK_PTR(RenderService) RenderServiceWeakPtr;

#endif //RENDER_SERVICE_H