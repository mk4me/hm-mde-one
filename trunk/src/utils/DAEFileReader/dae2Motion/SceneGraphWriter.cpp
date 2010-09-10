#include "stdafx.h"

#include "BaseWriter.h"
#include "SceneGraphWriter.h"
#include "dae2Motion.h"

//------------------------------
CSceneGraphWriter::CSceneGraphWriter( CDAE2MotionWriter* ogreWriter, const COLLADAFW::VisualScene& visualScene, const CDAE2MotionWriter::LibraryNodesList& libraryNodesList )
: CBaseWriter( ogreWriter )
, mVisualScene(visualScene)
, mLibraryNodesList(libraryNodesList)
{
}

//------------------------------
CSceneGraphWriter::~CSceneGraphWriter()
{
}

//------------------------------
bool CSceneGraphWriter::write(  )
{
    createUniqueIdNodeMap();

    NodeInfo nodeInfo( COLLADABU::Math::Matrix4::IDENTITY );
    mNodeInfoStack.push( nodeInfo );

    writeNodes( mVisualScene.getRootNodes());
    return true;
}

//------------------------------
bool CSceneGraphWriter::writeNodes( const COLLADAFW::NodePointerArray& nodesToWriter )
{
    for ( size_t i = 0, count = nodesToWriter.getCount(); i < count; ++i)
    {
        writeNode(nodesToWriter[i]);
    }
    return true;
}

//------------------------------
bool CSceneGraphWriter::writeNode(const COLLADAFW::Node* nodeToWriter )
{
    const NodeInfo& parentNodeInfo = mNodeInfoStack.top();
    const COLLADABU::Math::Matrix4& parentWorldMatrix = parentNodeInfo.worldTransformation;

    COLLADABU::Math::Matrix4 worldMatrix = parentWorldMatrix;
    COLLADABU::Math::Matrix4 pivot = COLLADABU::Math::Matrix4::IDENTITY;

    // jesli to jest pivot
    if( isPivot(nodeToWriter) )
    {
        pivot = nodeToWriter->getTransformationMatrix();
    }
    //jesli nie pivot
    else
    {
        worldMatrix = parentWorldMatrix * nodeToWriter->getTransformationMatrix();
    }

    NodeInfo nodeInfo( worldMatrix, pivot );
    mNodeInfoStack.push(nodeInfo);

    writeNodes(nodeToWriter->getChildNodes());


    storeInstanceGeometries( nodeToWriter->getInstanceGeometries(), worldMatrix, pivot);
    storeInstanceLights(nodeToWriter->getInstanceLights(), worldMatrix);
    storeInstanceCameras(nodeToWriter->getInstanceCameras(), worldMatrix);

    writeInstanceNodes( nodeToWriter->getInstanceNodes() );

    mNodeInfoStack.pop();

    return true;
}

//------------------------------
void CSceneGraphWriter::storeInstanceLights(const COLLADAFW::InstanceLightPointerArray& instanceLights,
                                            const COLLADABU::Math::Matrix4& worldMatrix)
{
    for (size_t i = 0, count = instanceLights.getCount(); i < count; ++i)
    {
        COLLADAFW::InstanceLight* instanceLight = instanceLights[i];
        CDAE2MotionWriter::InstanceLightInfo instanceLightInfo(instanceLight, worldMatrix);
        addLightUniqueIdInstanceLightInfoPair(instanceLight->getInstanciatedObjectId(), instanceLightInfo);
    }
}

//------------------------------
void CSceneGraphWriter::storeInstanceCameras(const COLLADAFW::InstanceCameraPointerArray& instanceCameras,
                                             const COLLADABU::Math::Matrix4& worldMatrix)
{
    for (size_t i = 0, count = instanceCameras.getCount(); i < count; ++i)
    {
        COLLADAFW::InstanceCamera* instanceCamera = instanceCameras[i];
        CDAE2MotionWriter::InstanceCameraInfo instanceCameraInfo(instanceCamera, worldMatrix);
        addCameraUniqueIdInstanceCameraInfoPair(instanceCamera->getInstanciatedObjectId(), instanceCameraInfo);
    }
}

//------------------------------
void CSceneGraphWriter::storeInstanceGeometries( const COLLADAFW::InstanceGeometryPointerArray& instanceGeometries, 
                                                const COLLADABU::Math::Matrix4& worldMatrix, const COLLADABU::Math::Matrix4& pivot  )
{
    for ( size_t i = 0, count = instanceGeometries.getCount(); i < count; ++i)
    {
        COLLADAFW::InstanceGeometry* instanceGeometry = instanceGeometries[i];
        CDAE2MotionWriter::InstanceGeometryInfo instanceGeometryInfo( instanceGeometry, worldMatrix, pivot);
        addGeometryUniqueIdInstanceGeometryInfoPair(instanceGeometry->getInstanciatedObjectId(), instanceGeometryInfo);
    }
}

//------------------------------
// void CSceneGraphWriter::storeInstanceController( const COLLADAFW::InstanceControllerPointerArray& instanceController, 
//                                                 const COLLADABU::Math::Matrix4& worldMatrix, const COLLADABU::Math::Matrix4& pivot )
// {
//     for ( size_t i = 0, count = instanceController.getCount(); i < count; ++i)
//     {
//         COLLADAFW::InstanceController* instanceGeometry = instanceController[i];
// 
//         CDAE2MotionWriter::InstanceGeometryInfo instanceGeometryInfo( instanceGeometry, worldMatrix, pivot);
//         addGeometryUniqueIdInstanceGeometryInfoPair(instanceGeometry->getInstanciatedObjectId(), instanceGeometryInfo);
//     }
// }

//------------------------------
void CSceneGraphWriter::writeInstanceNodes( const COLLADAFW::InstanceNodePointerArray& instanceNodes)
{
    for ( size_t i = 0, count = instanceNodes.getCount(); i < count; ++i)
    {
        const COLLADAFW::InstanceNode* instanceNode = instanceNodes[i];
        const COLLADAFW::UniqueId& referencedNodeUniqueId = instanceNode->getInstanciatedObjectId();
        UniqueIdNodeMap::const_iterator it = mUniqueIdNodeMap.find( referencedNodeUniqueId );
        if ( it != mUniqueIdNodeMap.end() )
        {
            const COLLADAFW::Node* referencedNode = it->second;
            writeNode( referencedNode );
        }
    }
}

//------------------------------
void CSceneGraphWriter::createUniqueIdNodeMap( COLLADAFW::Node* node )
{
    mUniqueIdNodeMap[node->getUniqueId()] = node;
    createUniqueIdNodeMap( node->getChildNodes() );
}

//------------------------------
void CSceneGraphWriter::createUniqueIdNodeMap( const COLLADAFW::NodePointerArray& nodes )
{
    for ( size_t i = 0, cout = nodes.getCount(); i < cout; ++i)
    {
        COLLADAFW::Node* node = nodes[i];
        createUniqueIdNodeMap( node );
    }
}

//------------------------------
void CSceneGraphWriter::createUniqueIdNodeMap()
{
    createUniqueIdNodeMap( mVisualScene.getRootNodes() );
    CDAE2MotionWriter::LibraryNodesList::const_iterator it = mLibraryNodesList.begin();
    for ( ; it != mLibraryNodesList.end(); ++it )
    {
        const COLLADAFW::LibraryNodes& libraryNodes = *it;
        createUniqueIdNodeMap( libraryNodes.getNodes() );
    }
}

bool CSceneGraphWriter::isPivot( const COLLADAFW::Node* node )
{
    //opencollada
    if(node->getName().empty() && node->getOriginalId().empty())
        return true;
    //collada max
    else if(COLLADAFW::String::npos != node->getName().rfind("-Pivot"))
        return true;
    //nie pivot
    else
        return false;
}
