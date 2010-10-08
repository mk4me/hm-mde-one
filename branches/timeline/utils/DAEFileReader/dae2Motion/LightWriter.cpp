#include "stdafx.h"
#include "LightWriter.h"
#include "DAE2MotionWriter.h"

#include "COLLADAFWLight.h"
#include "COLLADAFWUniqueId.h"
#include "COLLADAFWTriangles.h"
#include "COLLADAFWTrifans.h"
#include "COLLADAFWTristrips.h"

#include "COLLADABUPlatform.h"

#ifdef COLLADABU_OS_WIN
#	include <windows.h>
#endif


CLightWriter::CLightWriter(CDAE2MotionWriter* cubeITMWriter, const COLLADAFW::Light* light )
: CBaseWriter(cubeITMWriter)
, mLight(light)
{
}

CLightWriter::~CLightWriter(void)
{
}

bool CLightWriter::write(CILight** ppITMLight)
{
    COLLADABU::Math::Quaternion mCurrentRotationQuaternion;
    // Nowe ITM Light
    m_CurrentLight = new CILight();

    const COLLADAFW::UniqueId& lightUniqueId = mLight->getUniqueId();
    const CDAE2MotionWriter::LightUniqueIdInstanceLightInfoMap& instanceLightMap = getLightUniqueIdInstanceGeometryInfoMap();
    CDAE2MotionWriter::LightUniqueIdInstanceLightInfoMap::const_iterator lowerBound = instanceLightMap.lower_bound(lightUniqueId);
    CDAE2MotionWriter::LightUniqueIdInstanceLightInfoMap::const_iterator upperBound = instanceLightMap.upper_bound(lightUniqueId);

    for ( CDAE2MotionWriter::LightUniqueIdInstanceLightInfoMap::const_iterator it = lowerBound; it != upperBound; ++it)
    {
        const CDAE2MotionWriter::InstanceLightInfo& instanceLightInfo = it->second;
        const COLLADABU::Math::Matrix4& worldMatrix = instanceLightInfo.worldMatrix;
        //const COLLADAFW::MeshPrimitiveArray& meshPrimitives = mMesh->getMeshPrimitives();
        const COLLADAFW::InstanceLight* instanceLight = instanceLightInfo.fwInstanceLight;

        worldMatrix.extract3x3Matrix(mCurrentRotationMatrix );
        mCurrentRotationQuaternion = worldMatrix.extractQuaternion();
        mCurrentTranslationVector.set(worldMatrix[0][3], worldMatrix[1][3], worldMatrix[2][3]);
    }	

    // zapisujemy informacjê o nazwie i identyfikatorze
    m_CurrentLight->m_Name = mLight->getName();
    m_CurrentLight->m_OriginalId = mLight->getOriginalId();

    // Ustawienia dotycz¹ce œwiat³a typu reflektor (spot light)
    m_CurrentLight->m_AttenuationConst = mLight->getConstantAttenuation();
    m_CurrentLight->m_AttenuationLinear = mLight->getLinearAttenuation();
    m_CurrentLight->m_AttenuationQuad = mLight->getQuadraticAttenuation();

    m_CurrentLight->m_SpotFalloff = mLight->getFallOffExponent();
    m_CurrentLight->m_SpotInner = mLight->getFallOffAngle();
    // m_CurrentLight->m_SpotOuter do wyjaœnienias

    // Ustawiamy kolor
    m_CurrentLight->m_Diffuse.push_back((float)mLight->getColor().getRed());
    m_CurrentLight->m_Diffuse.push_back((float)mLight->getColor().getGreen());
    m_CurrentLight->m_Diffuse.push_back((float)mLight->getColor().getBlue());

    //const float* positionsArray = mCurrentTranslationVector.
    //COLLADABU::Math::Vector3 position(positionsArray[0], positionsArray[1], positionsArray[2]);

    // Ustawiamy pozycjê
    m_CurrentLight->m_Positions.push_back((float) mCurrentTranslationVector.x);
    m_CurrentLight->m_Positions.push_back((float) mCurrentTranslationVector.y);
    m_CurrentLight->m_Positions.push_back((float) mCurrentTranslationVector.z);

    // to samo pod cube
    m_CurrentLight->m_Translation[0] = (float) mCurrentTranslationVector.x;
    m_CurrentLight->m_Translation[1] = (float) mCurrentTranslationVector.y;
    m_CurrentLight->m_Translation[2] = (float) mCurrentTranslationVector.z;

    // Ustawiamy rotacjê
    m_CurrentLight->m_Direction.push_back((float) mCurrentRotationQuaternion.x);
    m_CurrentLight->m_Direction.push_back((float) mCurrentRotationQuaternion.y);
    m_CurrentLight->m_Direction.push_back((float) mCurrentRotationQuaternion.z);
    m_CurrentLight->m_Direction.push_back((float) mCurrentRotationQuaternion.w);

    // to samo dla Cube
    m_CurrentLight->m_Rotation[0] = (float) mCurrentRotationQuaternion.x;
    m_CurrentLight->m_Rotation[1] = (float) mCurrentRotationQuaternion.y;
    m_CurrentLight->m_Rotation[2] = (float) mCurrentRotationQuaternion.z;
    m_CurrentLight->m_Rotation[3] = (float) mCurrentRotationQuaternion.w;

    switch (mLight->getLightType())
    {
    case mLight->AMBIENT_LIGHT:
        m_CurrentLight->m_LightType = ELightType::AmbientLight;
        break;

    case mLight->DIRECTIONAL_LIGHT:
        m_CurrentLight->m_LightType = ELightType::DirectionalLight;
        break;

    case mLight->POINT_LIGHT:
        m_CurrentLight->m_LightType = ELightType::PointLight;
        break;

    case mLight->SPOT_LIGHT:
        m_CurrentLight->m_LightType = ELightType::SpotLight;
        break;

        //default:
        // coœ nie tak
    };
    *ppITMLight = m_CurrentLight;
    return TRUE;
}