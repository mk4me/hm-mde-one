#ifndef I_RENDER_SERVICE_H
#define I_RENDER_SERVICE_H

#include <osg/Camera>
#include <osgGA/CameraManipulator>

#include <osgViewer/GraphicsWindow>
#include <osgViewer/View>

//#include <osgui/QOsgWidgets.h>
#include <core/IService.h>



class IC3DModel;
class Mesh;
class IModel;
class IFactor;

//--------------------------------------------------------------------------------------------------
// Render Service
//--------------------------------------------------------------------------------------------------

typedef osg::ref_ptr<osg::Group> osgGroupPtr;

class IRenderService: public core::IService
{
public:
    virtual ~IRenderService() {};

    virtual void TestScreenForNewModel(IModel* model) = 0;
    virtual void AddObjectToRender(osg::Node* node) = 0;
    virtual void SetScene(osg::Group* root) = 0;
    virtual void Clear() = 0;


    virtual osgViewer::Scene* GetMainWindowScene() = 0;
    virtual osgGroupPtr GetRoot() = 0;

    virtual osg::Light* GetLight() = 0;

    virtual void RenderC3D(IC3DModel* c3dmodel) = 0;  

    virtual void EnableMesh() = 0;
    virtual void DisableMesh() = 0;

    virtual void EnableBone() = 0;
    virtual void DisableBone() = 0;

    virtual void EnableMarker() = 0;
    virtual void DisableMarker() = 0;

    virtual void SetC3DMarkerToRender(IC3DModel *c3dmarker) = 0;
    virtual IFactor* GetFactory() = 0;

    virtual void CreatingAndRenderMarkerPath(IC3DModel* c3dModel, std::vector<int > markerSelectedList) = 0;

// nowy intefejs
public:

    //! Dodaje kamerê.
    //! \param name Nazwa kamery.
    //! \param camera Kamera.
    //! \param manipulator Manipulator kamery.
    virtual void addView(osg::Camera* camera, osgGA::CameraManipulator* manipulator) = 0;

};


#endif //I_RENDER_SERVICE_H