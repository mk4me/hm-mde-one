#ifndef RENDER_SERVICE_H
#define RENDER_SERVICE_H

#include <iostream>
#include <vector>

#include "../include/GlobalServicesIDs.h"
#include "../include/SimpleFunctors.h"
#include <osg//Node>
#include <osgViewer/GraphicsWindow>
#include <osgViewer/View>

#include "AdapterWidget.h"

#include <map>

class Model;
class Mesh;
class UserInterfaceService;
//--------------------------------------------------------------------------------------------------
// Render Service
//--------------------------------------------------------------------------------------------------

typedef osg::ref_ptr<osg::Group> osgGroupPtr;

class RenderService: public IBaseService
{
public:
    RenderService();
    virtual ~RenderService();
    virtual AsyncResult OnTick(double delta); 
    virtual AsyncResult OnAdded(IServiceManager* serviceManager); 


    void TestScreenForNewModel(Model* model);
    void AddObjectToRender(osg::Node* node);
    void SetScene(osg::Group* root);
    void Clear();

    bool CreateNewWindow(std::string windowName);

    osgViewer::Scene* GetMainWindowScene();
    osgGroupPtr GetRoot();
    ViewerQT* GetMainAdapterWidget();

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