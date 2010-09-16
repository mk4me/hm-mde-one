#ifndef DEA2MOTIONWRITER_H
#define DEA2MOTIONWRITER_H

#include "dae2Motion.h"

class CDAE2MotionWriter : public COLLADAFW::IWriter
{
public:
    enum Runs
    {
        SCENEGRAPH_RUN,
        GEOMETRY_RUN
    };

    typedef std::list<COLLADAFW::LibraryNodes> LibraryNodesList;

    struct InstanceGeometryInfo
    {
        InstanceGeometryInfo( COLLADAFW::InstanceGeometry* _fwInstanceGeometry, const COLLADABU::Math::Matrix4& _worldMatrix,
            const COLLADABU::Math::Matrix4& _pivot = COLLADABU::Math::Matrix4::IDENTITY)
            :  fwInstanceGeometry(_fwInstanceGeometry), worldMatrix(_worldMatrix), pivot(_pivot) {}
        COLLADAFW::InstanceGeometry* fwInstanceGeometry;
        COLLADABU::Math::Matrix4 worldMatrix;
        COLLADABU::Math::Matrix4 pivot;
    };

    struct InstanceLightInfo
    {
        InstanceLightInfo( COLLADAFW::InstanceLight* _fwInstanceLight, const COLLADABU::Math::Matrix4& _worldMatrix)
            :  fwInstanceLight(_fwInstanceLight), worldMatrix(_worldMatrix) {}
        COLLADAFW::InstanceLight* fwInstanceLight;
        COLLADABU::Math::Matrix4 worldMatrix;
    };

    struct InstanceCameraInfo
    {
        InstanceCameraInfo( COLLADAFW::InstanceCamera* _fwInstanceCamera, const COLLADABU::Math::Matrix4& _worldMatrix)
            :  fwInstanceCamera(_fwInstanceCamera), worldMatrix(_worldMatrix) {}
        COLLADAFW::InstanceCamera* fwInstanceCamera;
        COLLADABU::Math::Matrix4 worldMatrix;
    };

    typedef std::multimap<COLLADAFW::UniqueId, InstanceLightInfo> LightUniqueIdInstanceLightInfoMap;

    typedef std::multimap<COLLADAFW::UniqueId, InstanceCameraInfo> CameraUniqueIdInstanceCameraInfoMap;

    typedef std::multimap<COLLADAFW::UniqueId, InstanceGeometryInfo> GeometryUniqueIdInstanceGeometryInfoMap;

    /** Maps unique ids of framework materials to the corresponding framework material.*/
    typedef std::map<COLLADAFW::UniqueId, COLLADAFW::Material> UniqueIdFWMaterialMap;

    /** Maps unique ids of framework images to the corresponding framework image.*/
    typedef std::map<COLLADAFW::UniqueId, COLLADAFW::Image> UniqueIdFWImageMap;

    /** Maps unique ids of framework effects to the corresponding framework material.*/
    typedef std::map<COLLADAFW::UniqueId, COLLADAFW::Effect> UniqueIdFWEffectMap;

    enum Severity
    {
        SEVERITY_INFORMATION,
        SEVERITY_WARNING,
        _SEVERITY_ERROR
    };


private:
    COLLADABU::URI mInputFile;
    COLLADABU::URI mOutputFile;
    Runs mCurrentRun;

    SDea2Motion* m_IOData;

    const COLLADAFW::VisualScene* mVisualScene;
    LibraryNodesList mLibrayNodesList;

    GeometryUniqueIdInstanceGeometryInfoMap mGeometryUniqueIdInstanceGeometryInfoMap;

    LightUniqueIdInstanceLightInfoMap mLightUniqueIdInstanceLightInfoMap;

    CameraUniqueIdInstanceCameraInfoMap mCameraUniqueIdInstanceCameraInfoMap;

    UniqueIdFWMaterialMap mUniqueIdFWMaterialMap;
    UniqueIdFWImageMap mUniqueIdFWImageMap;
    UniqueIdFWEffectMap mUniqueIdFWEffectMap;

public:
    // CDEA2CubeITMWriter( const COLLADABU::URI& inputFile, const COLLADABU::URI& outputFile );
    CDAE2MotionWriter( SDea2Motion* io_data );

    virtual ~CDAE2MotionWriter();

    // public:
    //    static void reportError(const STRING& method, const STRING& message, Severity severity);

public:

    bool write();

    /** Deletes the entire scene.
    @param errorMessage A message containing informations about the error that occurred.
    */
    void cancel(const std::string& errorMessage);

    /** Prepare to receive data.*/
    void start();

    /** Remove all objects that don't have an object. Deletes unused visual scenes.*/
    void finish();

    /** When this method is called, the writer must write the global document asset.
    @return The writer should return true, if writing succeeded, false otherwise.*/
    virtual bool writeGlobalAsset ( const COLLADAFW::FileInfo* asset );

    /** Writes the entire visual scene.
    @return True on succeeded, false otherwise.*/
    virtual bool writeVisualScene ( const COLLADAFW::VisualScene* visualScene );

    /** Writes the scene.
    @return True on succeeded, false otherwise.*/
    virtual bool writeScene ( const COLLADAFW::Scene* scene );

    /** Handles all nodes in the library nodes.
    @return True on succeeded, false otherwise.*/
    virtual bool writeLibraryNodes( const COLLADAFW::LibraryNodes* libraryNodes );

    /** Writes the geometry.
    @return True on succeeded, false otherwise.*/
    virtual bool writeGeometry ( const COLLADAFW::Geometry* geometry );

    /** Writes the material.
    @return True on succeeded, false otherwise.*/
    virtual bool writeMaterial( const COLLADAFW::Material* material );

    /** Writes the effect.
    @return True on succeeded, false otherwise.*/
    virtual bool writeEffect( const COLLADAFW::Effect* effect );

    /** Writes the camera.
    @return True on succeeded, false otherwise.*/
    virtual bool writeCamera( const COLLADAFW::Camera* camera );

    /** Writes the image.
    @return True on succeeded, false otherwise.*/
    virtual bool writeImage( const COLLADAFW::Image* image );

    /** Writes the light.
    @return True on succeeded, false otherwise.*/
    virtual bool writeLight( const COLLADAFW::Light* light );

    /** Writes the animation.
    @return True on succeeded, false otherwise.*/
    virtual bool writeAnimation( const COLLADAFW::Animation* animation );

    /** Writes the animation.
    @return True on succeeded, false otherwise.*/
    virtual bool writeAnimationList( const COLLADAFW::AnimationList* animationList );

    /** Writes the skin controller data.
    @return True on succeeded, false otherwise.*/
    virtual bool writeSkinControllerData( const COLLADAFW::SkinControllerData* skinControllerData );

    /** Writes the controller.
    @return True on succeeded, false otherwise.*/
    virtual bool writeController( const COLLADAFW::Controller* Controller );

    /** When this method is called, the writer must write the formulas. All the formulas of the entire
    COLLADA file are contained in @a formulas.
    @return The writer should return true, if writing succeeded, false otherwise.*/
    virtual bool writeFormulas( const COLLADAFW::Formulas* formulas ){return true;}

    /** When this method is called, the writer must write the kinematics scene. 
    @return The writer should return true, if writing succeeded, false otherwise.*/
    virtual bool writeKinematicsScene( const COLLADAFW::KinematicsScene* kinematicsScene ){return true;};



protected:
    friend class CBaseWriter;

    GeometryUniqueIdInstanceGeometryInfoMap& getGeometryUniqueIdInstanceGeometryInfoMap() { return mGeometryUniqueIdInstanceGeometryInfoMap; }

    LightUniqueIdInstanceLightInfoMap& getLightUniqueIdInstanceLightInfoMap() { return mLightUniqueIdInstanceLightInfoMap; }

    CameraUniqueIdInstanceCameraInfoMap& getCameraUniqueIdInstanceCameraInfoMap() { return mCameraUniqueIdInstanceCameraInfoMap; }

    UniqueIdFWMaterialMap& getUniqueIdFWMaterialMap() { return mUniqueIdFWMaterialMap; }

    UniqueIdFWImageMap& getUniqueIdFWImageMap() { return mUniqueIdFWImageMap; }

    UniqueIdFWEffectMap& getUniqueIdFWEffectMap() { return mUniqueIdFWEffectMap; }

    const COLLADABU::URI& getInputFile() const { return mInputFile; }

    const COLLADABU::URI& getOutputFile() const { return mOutputFile; }


private:
    /** Disable default copy ctor. */
    CDAE2MotionWriter( const CDAE2MotionWriter& pre );
    /** Disable default assignment operator. */
    const CDAE2MotionWriter& operator= ( const CDAE2MotionWriter& pre );

};


#endif // DEA2MOTIONWRITER_H