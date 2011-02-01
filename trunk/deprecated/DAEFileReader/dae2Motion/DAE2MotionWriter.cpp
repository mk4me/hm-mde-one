#include "stdafx.h"

#include "DAE2MotionWriter.h"
#include "MeshWriter.h"
#include "LightWriter.h"
#include "CameraWriter.h"
#include "SceneGraphWriter.h"

// TODO:
// #include "DEA2CubeITMSceneGraphWriter.h"

#include "COLLADASaxFWLLoader.h"
#include "COLLADAFWRoot.h"
#include "COLLADAFWGeometry.h"
#include "COLLADAFWNode.h"
#include "COLLADAFWVisualScene.h"
#include "COLLADAFWInstanceGeometry.h"

CDAE2MotionWriter::CDAE2MotionWriter( SDea2Motion* io_data ):
IWriter()
, mCurrentRun(SCENEGRAPH_RUN)
, mVisualScene(0)
, mOutputFile()
//, mInputFile()
, mInputFile(io_data->in_fileName.c_str())
{
    //mOutputFile.reset()
    //mInputFile.reset();
    m_IOData = io_data;


    // zamiana przecinkow na kropki
    for(unsigned int i = 0; i < m_IOData->in_length; ++i)
    {
        if(',' == m_IOData->in_buffer[i])
        {
            m_IOData->in_buffer[i] = '.';
        }
    }
}

//--------------------------------------------------------------------
CDAE2MotionWriter::~CDAE2MotionWriter()
{
    delete mVisualScene;
}

//--------------------------------------------------------------------
/*
void CDEA2CubeITMWriter::reportError( const String& method, const String& message, Severity severity )
{
switch ( severity )
{
case SEVERITY_INFORMATION:
std::cout << "Information"; 
break;
case SEVERITY_WARNING:
std::cout << "Warning"; 
break;
case _SEVERITY_ERROR:
std::cout << "Error"; 
break;
}
if ( !method.empty() )
std::cout << " in " << method;
std::cout << ": ";
std::cout << message << std::endl;
}
*/

//--------------------------------------------------------------------
bool CDAE2MotionWriter::write()
{
    COLLADASaxFWL::Loader loader;
    COLLADAFW::Root root(&loader, this);
    std::vector<IILight*>::iterator itLight;


    // Load scene graph 
    if ( !root.loadDocument(mInputFile.toNativePath(), m_IOData->in_buffer, (int)m_IOData->in_length, true))
        return false;

    // if there is no visual scene in the COLLADA file, nothing to export here
    // TODO
    if ( mVisualScene )
    {
        CSceneGraphWriter sceneGraphWriter(this, *mVisualScene, mLibrayNodesList);
        sceneGraphWriter.write();
    }

    // load and write geometries
    mCurrentRun = GEOMETRY_RUN;

    if ( !root.loadDocument(mInputFile.toNativePath(), m_IOData->in_buffer, (int)m_IOData->in_length, true))
        return false;

    return true;
}

//--------------------------------------------------------------------
void CDAE2MotionWriter::cancel( const std::string& errorMessage )
{
}

//--------------------------------------------------------------------
void CDAE2MotionWriter::start()
{
}

//--------------------------------------------------------------------
void CDAE2MotionWriter::finish()
{
}

//--------------------------------------------------------------------
bool CDAE2MotionWriter::writeGlobalAsset( const COLLADAFW::FileInfo* asset )
{
    if ( mCurrentRun != SCENEGRAPH_RUN )
        return true;
    return true;
}

//--------------------------------------------------------------------
bool CDAE2MotionWriter::writeVisualScene( const COLLADAFW::VisualScene* visualScene )
{
    if ( mCurrentRun != SCENEGRAPH_RUN )
        return true;
    // todo. handle multiple occurences of visual scene properly. This is just a quick fix
    if ( mVisualScene )
    {
        delete mVisualScene;
    }
    mVisualScene = new COLLADAFW::VisualScene(*visualScene);
    return true;
}

//--------------------------------------------------------------------
bool CDAE2MotionWriter::writeScene( const COLLADAFW::Scene* scene )
{
    return true;
}

//--------------------------------------------------------------------
bool CDAE2MotionWriter::writeLibraryNodes( const COLLADAFW::LibraryNodes* libraryNodes )
{
    if ( mCurrentRun != SCENEGRAPH_RUN )
        return true;
    mLibrayNodesList.push_back(*libraryNodes);
    return true;
}

//--------------------------------------------------------------------
bool CDAE2MotionWriter::writeGeometry( const COLLADAFW::Geometry* geometry )
{
    if ( mCurrentRun != GEOMETRY_RUN )
        return true;
    if ( geometry->getType() != COLLADAFW::Geometry::GEO_TYPE_MESH )
        return true;

    CMeshWriter meshWriter( this, (COLLADAFW::Mesh*)geometry );
    CIMesh* pMesh = NULL;
    meshWriter.write(&pMesh);
    m_IOData->out_meshes.push_back(pMesh);

    return TRUE;
}

//--------------------------------------------------------------------
bool CDAE2MotionWriter::writeMaterial( const COLLADAFW::Material* material )
{
    if ( mCurrentRun != SCENEGRAPH_RUN )
        return true;
    mUniqueIdFWMaterialMap.insert(std::make_pair(material->getUniqueId(),*material ));
    return true;
}

//--------------------------------------------------------------------
bool CDAE2MotionWriter::writeEffect( const COLLADAFW::Effect* effect )
{
    if ( mCurrentRun != SCENEGRAPH_RUN )
        return true;
    mUniqueIdFWEffectMap.insert(std::make_pair(effect->getUniqueId(),*effect ));
    return true;
}

//--------------------------------------------------------------------
bool CDAE2MotionWriter::writeCamera( const COLLADAFW::Camera* camera )
{
    if ( mCurrentRun == SCENEGRAPH_RUN )
        return true;

    if ( mCurrentRun != GEOMETRY_RUN )
        return true;

    CCameraWriter cameraWriter(this, (COLLADAFW::Camera*)camera);
    CICamera* pCamera = NULL;
    cameraWriter.write(&pCamera);
    m_IOData->out_cameras.push_back(pCamera);
    return true;
}

//--------------------------------------------------------------------
bool CDAE2MotionWriter::writeImage( const COLLADAFW::Image* image )
{
    if ( mCurrentRun != SCENEGRAPH_RUN )
        return true;
    mUniqueIdFWImageMap.insert(std::make_pair(image->getUniqueId(),*image ));
    return true;
}

//--------------------------------------------------------------------
bool CDAE2MotionWriter::writeLight( const COLLADAFW::Light* light )
{
    if ( mCurrentRun == SCENEGRAPH_RUN )
        return true;

    if ( mCurrentRun != GEOMETRY_RUN )
        return true;

    CLightWriter lightWriter(this, (COLLADAFW::Light*)light);
    CILight* pLight = NULL;
    lightWriter.write(&pLight);
    m_IOData->out_lights.push_back(pLight);
    return true;
}

//--------------------------------------------------------------------
bool CDAE2MotionWriter::writeAnimation( const COLLADAFW::Animation* animation )
{
    return true;
}

//--------------------------------------------------------------------
bool CDAE2MotionWriter::writeAnimationList( const COLLADAFW::AnimationList* animationList )
{
    return true;
}

//--------------------------------------------------------------------
bool CDAE2MotionWriter::writeSkinControllerData( const COLLADAFW::SkinControllerData* skinControllerData )
{
    return true;
}

//--------------------------------------------------------------------
bool CDAE2MotionWriter::writeController( const COLLADAFW::Controller* Controller )
{
    return true;
}