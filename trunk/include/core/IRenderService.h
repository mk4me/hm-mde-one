#ifndef I_RENDER_SERVICE_H
#define I_RENDER_SERVICE_H


#include <osgViewer/GraphicsWindow>
#include <osgViewer/View>

#include <core/QOSGWidget.h>


class IC3DModel;
class Mesh;
class IModel;
class IFactor;

//--------------------------------------------------------------------------------------------------
// Render Service
//--------------------------------------------------------------------------------------------------

typedef osg::ref_ptr<osg::Group> osgGroupPtr;

class IRenderService: public IService
{
public:
    virtual ~IRenderService() {};

    virtual void TestScreenForNewModel(IModel* model) = 0;
    virtual void AddObjectToRender(osg::Node* node) = 0;
    virtual void SetScene(osg::Group* root) = 0;
    virtual void Clear() = 0;


    virtual osgViewer::Scene* GetMainWindowScene() = 0;
    virtual osgGroupPtr GetRoot() = 0;
    virtual QOSGViewer* GetMainAdapterWidget() = 0;

    virtual osg::Light* GetLight() = 0;

    virtual void RenderBone(IModel* model) = 0;       // TODO: prawdopodobnie usunac t�metode
    virtual void RenderC3D(IC3DModel* c3dmodel) = 0;  // TODO: prawdopodobnie usunac t�metode

    virtual void DrawNormals(IModel* model, float size) = 0; // TODO: prawdopodobnie usunac t�metode

    virtual void EnableMesh() = 0;
    virtual void DisableMesh() = 0;

    virtual void EnableBone() = 0;
    virtual void DisableBone() = 0;

    virtual void EnableMarker() = 0;
    virtual void DisableMarker() = 0;

    virtual void SetC3DMarkerToRender(IC3DModel *c3dmarker) = 0;
    virtual IFactor* GetFactory() = 0;

};


#endif //I_RENDER_SERVICE_H