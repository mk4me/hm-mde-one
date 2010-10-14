#ifndef RENDER_SERVICE_H
#define RENDER_SERVICE_H

#include <iostream>
#include <vector>

#include <core/GlobalServicesIDs.h>
#include <core/SimpleFunctors.h>
#include <osg//Node>
#include <osgViewer/GraphicsWindow>
#include <osgViewer/View>

#include "AdapterWidget.h"

#include <map>

class Model;
class Mesh;
class IModel;
class IDataManager;

class UserInterfaceService;
//--------------------------------------------------------------------------------------------------
// Render Service
//--------------------------------------------------------------------------------------------------

typedef osg::ref_ptr<osg::Group> osgGroupPtr;

class RenderService: public IBaseService
{
    UNIQUE_ID('REND','SRVC');
public:
    RenderService();
    virtual ~RenderService();
    virtual AsyncResult OnAdded(IServiceManager* serviceManager); 

    virtual void SetModel(IDataManager* dataManager);

    void TestScreenForNewModel(Model* model);
    void AddObjectToRender(osg::Node* node);
    void SetScene(osg::Group* root);
    void Clear();

    bool CreateNewWindow(std::string windowName);

    osgViewer::Scene* GetMainWindowScene();
    osgGroupPtr GetRoot();
    ViewerQT* GetMainAdapterWidget();

	void RenderBone(Model* model);

private: 
    M_DECLARE_CLASS(); 
    void Inicialize();
    void InicizlizeModelMesh(Model* model);

    UserInterfaceService* m_pUserInterfaceService;

    std::map<std::string, ViewerQT*> m_osgViewMap;
    std::map<std::string, QWidget*> m_widgetMap;

    ViewerQT* m_pMainOsgView; 
    osgGroupPtr m_spRoot;

    ServiceManager* m_pServiceManager;
};


#endif //RENDER_SERVICE_H