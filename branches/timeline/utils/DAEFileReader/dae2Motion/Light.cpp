#include "StdAfx.h"
#include "Light.h"

#include "dae2Motion.h"

// Konstruktor i destruktor
CILight::CILight(void)
{
}

CILight::~CILight(void)
{
}

std::string CILight::GetOriginalId()
{
    return m_OriginalId;
}

std::string CILight::GetName()
{
    return m_Name;
}

ELightType::TYPE CILight::GetLightType()
{
    return m_LightType;
}

float* CILight::GetPositions()
{
    if (m_Positions.size())
        return &(m_Positions[0]);
    return NULL;
}

float* CILight::GetTranslation()
{
    return m_Translation;
}	

float* CILight::GetRotation()
{
    return m_Rotation;
}

float* CILight::GetDiffuse()
{
    if (m_Diffuse.size())
        return &(m_Diffuse[0]);
    return NULL;
}

float* CILight::GetSpecular()
{
    if (m_Specular.size())
        return &(m_Specular[0]);
    return NULL;
}

float* CILight::GetDirection()
{
    if (m_Direction.size())
        return &(m_Direction[0]);
    return NULL;
}

float* CILight::GetAttenuationConstant()
{
    return &(m_AttenuationConst);
}

float* CILight::GetAttenuationLinear()
{
    return &(m_AttenuationLinear);
}

float* CILight::GetAttenuationQuadric()
{
    return &(m_AttenuationQuad);
}

float* CILight::GetAttenuationRange()
{
    return &(m_Range);
}

float* CILight::GetSpotlightFalloff()
{
    return &(m_SpotFalloff);
}

float* CILight::GetSpotlightInnerAngle()
{
    return &(m_SpotInner);
}

float* CILight::GetSpotlightOuterAngle()
{
    return &(m_SpotOuter);
}


