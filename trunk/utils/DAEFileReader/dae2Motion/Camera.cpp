#include "StdAfx.h"
#include "Camera.h"

#include "dae2Motion.h"

// Konstruktor i destruktor
CICamera::CICamera(void)
{
}

CICamera::~CICamera(void)
{
}

std::string CICamera::GetOriginalId()
{
    return m_OriginalId;
}

std::string CICamera::GetName()
{
    return m_Name;
}

EProjectionType::TYPE CICamera::GetProjectionType()
{
    return m_ProjectionType;
}

float* CICamera::GetXFovOrXMag()
{
    return &(m_XFovOrXMag);
}

float* CICamera::GetYFovOrYMag()
{
    return &(m_YFovOrYMag);
}

float* CICamera::GetAspectRatio()
{
    return &(m_AspectRatio);
}

float* CICamera::GetNearClippingPlane()
{
    return &(m_NearClippingPlane);
}

float* CICamera::GetFarClippingPlane()
{
    return &(m_FarClippingPlane);
}

float* CICamera::GetPositions()
{
    if (m_Positions.size())
        return &(m_Positions[0]);
    return NULL;
}

float* CICamera::GetDirection()
{
    if (m_Direction.size())
        return &(m_Direction[0]);
    return NULL;
}

float* CICamera::GetTranslation()
{
    return m_Translation;
}

float* CICamera::GetRotation()
{
    return m_Rotation;
}