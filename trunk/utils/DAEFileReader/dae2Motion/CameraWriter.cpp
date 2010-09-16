#include "stdafx.h"
#include "CameraWriter.h"
#include "DAE2MotionWriter.h"

#include "COLLADAFWCamera.h"
#include "COLLADAFWUniqueId.h"
#include "COLLADAFWTriangles.h"
#include "COLLADAFWTrifans.h"
#include "COLLADAFWTristrips.h"

#include "COLLADABUPlatform.h"

#ifdef COLLADABU_OS_WIN
#	include <windows.h>
#endif


CCameraWriter::CCameraWriter(CDAE2MotionWriter* cubeITMWriter, const COLLADAFW::Camera* camera )
: CBaseWriter(cubeITMWriter)
, mCamera(camera)
{
}

CCameraWriter::~CCameraWriter(void)
{
}

bool CCameraWriter::write(CICamera** ppITMCamera)
{
    COLLADABU::Math::Quaternion mCurrentRotationQuaternion;

    // Nowe ITM Camera
    m_CurrentCamera = new CICamera();

    const COLLADAFW::UniqueId& cameraUniqueId = mCamera->getUniqueId();
    const CDAE2MotionWriter::CameraUniqueIdInstanceCameraInfoMap& instanceCameraMap = getCameraUniqueIdInstanceGeometryInfoMap();
    CDAE2MotionWriter::CameraUniqueIdInstanceCameraInfoMap::const_iterator lowerBound = instanceCameraMap.lower_bound(cameraUniqueId);
    CDAE2MotionWriter::CameraUniqueIdInstanceCameraInfoMap::const_iterator upperBound = instanceCameraMap.upper_bound(cameraUniqueId);

    for (CDAE2MotionWriter::CameraUniqueIdInstanceCameraInfoMap::const_iterator it = lowerBound; it != upperBound; ++it)
    {
        const CDAE2MotionWriter::InstanceCameraInfo& instanceCameraInfo = it->second;
        const COLLADABU::Math::Matrix4& worldMatrix = instanceCameraInfo.worldMatrix;
        const COLLADAFW::InstanceCamera* instanceCamera = instanceCameraInfo.fwInstanceCamera;

        worldMatrix.extract3x3Matrix(mCurrentRotationMatrix);
        mCurrentRotationQuaternion = worldMatrix.extractQuaternion();
        mCurrentTranslationVector.set(worldMatrix[0][3], worldMatrix[1][3], worldMatrix[2][3]);
    }

    // zapisujemy informacjê o nazwie i identyfikatorze
    m_CurrentCamera->m_OriginalId = mCamera->getOriginalId();
    m_CurrentCamera->m_Name = mCamera->getName();


    // Ustawienia dotycz¹ce œwiat³a typu reflektor (spot light)
    m_CurrentCamera->m_AspectRatio = mCamera->getAspectRatio();
    m_CurrentCamera->m_FarClippingPlane = mCamera->getFarClippingPlane();
    m_CurrentCamera->m_NearClippingPlane = mCamera->getNearClippingPlane();


    // Ustawiamy pozycjê
    m_CurrentCamera->m_Positions.push_back((float) mCurrentTranslationVector.x);
    m_CurrentCamera->m_Positions.push_back((float) mCurrentTranslationVector.y);
    m_CurrentCamera->m_Positions.push_back((float) mCurrentTranslationVector.z);

    // to samo pod cube
    m_CurrentCamera->m_Translation[0] = (float) mCurrentTranslationVector.x;
    m_CurrentCamera->m_Translation[1] = (float) mCurrentTranslationVector.y;
    m_CurrentCamera->m_Translation[2] = (float) mCurrentTranslationVector.z;

    // Ustawiamy rotacjê
    m_CurrentCamera->m_Direction.push_back((float) mCurrentRotationQuaternion.x);
    m_CurrentCamera->m_Direction.push_back((float) mCurrentRotationQuaternion.y);
    m_CurrentCamera->m_Direction.push_back((float) mCurrentRotationQuaternion.z);
    m_CurrentCamera->m_Direction.push_back((float) mCurrentRotationQuaternion.w);

    // to samo dla Cube
    m_CurrentCamera->m_Rotation[0] = (float) mCurrentRotationQuaternion.x;
    m_CurrentCamera->m_Rotation[1] = (float) mCurrentRotationQuaternion.y;
    m_CurrentCamera->m_Rotation[2] = (float) mCurrentRotationQuaternion.z;
    m_CurrentCamera->m_Rotation[3] = (float) mCurrentRotationQuaternion.w;

    switch (mCamera->getCameraType())
    {
    case mCamera->ORTHOGRAPHIC:
        m_CurrentCamera->m_ProjectionType = EProjectionType::Orthographic;
        m_CurrentCamera->m_XFovOrXMag = mCamera->getXMag();
        break;

    case mCamera->PERSPECTIVE:
        m_CurrentCamera->m_ProjectionType = EProjectionType::Perspective;
        m_CurrentCamera->m_XFovOrXMag = mCamera->getXFov();
        break;

        //default:
        // coœ nie tak
    };
    *ppITMCamera = m_CurrentCamera;
    return TRUE;
}