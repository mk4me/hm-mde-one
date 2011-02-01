#ifndef __SCENEGRAPHWRITER_H__
#define __SCENEGRAPHWRITER_H__

#include "BaseWriter.h"
#include "dae2Motion.h"

#include "Math/COLLADABUMathMatrix4.h"


class COLLADAFW::VisualScene;

class CSceneGraphWriter : public CBaseWriter 
{
private:
    struct NodeInfo
    {
        NodeInfo( COLLADABU::Math::Matrix4 _worldTransformation,
            COLLADABU::Math::Matrix4 _pivot = COLLADABU::Math::Matrix4::IDENTITY)
            : worldTransformation(_worldTransformation),
            pivot(_pivot)
        {
        }

        COLLADABU::Math::Matrix4 worldTransformation;
        //ADD pivot
        COLLADABU::Math::Matrix4 pivot;
    };

    typedef std::stack<NodeInfo> NodeInfoStack;

    typedef std::map<COLLADAFW::UniqueId, COLLADAFW::Node*> UniqueIdNodeMap;

private:
    const COLLADAFW::VisualScene& mVisualScene;
    const CDAE2MotionWriter::LibraryNodesList& mLibraryNodesList;
    NodeInfoStack mNodeInfoStack;
    UniqueIdNodeMap mUniqueIdNodeMap;

public:

    CSceneGraphWriter( CDAE2MotionWriter* ogreWriter, const COLLADAFW::VisualScene& visualScene, const CDAE2MotionWriter::LibraryNodesList& libraryNodesList );

    virtual ~CSceneGraphWriter();

    bool write();


private:

    /** Disable default copy ctor. */
    CSceneGraphWriter( const CSceneGraphWriter& pre );

    /** Disable default assignment operator. */
    const CSceneGraphWriter& operator= ( const CSceneGraphWriter& pre );

    void createUniqueIdNodeMap( COLLADAFW::Node* node );

    void createUniqueIdNodeMap( const COLLADAFW::NodePointerArray& nodes );

    void createUniqueIdNodeMap();

    bool writeNodes(  const COLLADAFW::NodePointerArray& nodesToWriter);

    bool writeNode(  const COLLADAFW::Node* nodeToWriter);

    void storeInstanceGeometries( const COLLADAFW::InstanceGeometryPointerArray& instanceGeometries, 
        const COLLADABU::Math::Matrix4& worldMatrix, const COLLADABU::Math::Matrix4& pivot = COLLADABU::Math::Matrix4::IDENTITY );

//     void storeInstanceController( const COLLADAFW::InstanceGeometryPointerArray& instanceGeometries, 
//         const COLLADABU::Math::Matrix4& worldMatrix, const COLLADABU::Math::Matrix4& pivot  );

    void storeInstanceLights( const COLLADAFW::InstanceLightPointerArray& instanceLights, 
        const COLLADABU::Math::Matrix4& worldMatrix );

    void storeInstanceCameras( const COLLADAFW::InstanceCameraPointerArray& instanceCameras, 
        const COLLADABU::Math::Matrix4& worldMatrix );

    void writeInstanceNodes( const COLLADAFW::InstanceNodePointerArray& instanceNodes);

    // PIVOT
    bool isPivot(const COLLADAFW::Node* node);
};


#endif // 