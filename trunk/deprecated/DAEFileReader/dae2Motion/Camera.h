#ifndef CAMERA_H
#define CAMERA_H
#include "dae2Motion.h"

class IICamera;
class CICamera;

class CICamera :
    public IICamera
{
    friend class CCameraWriter;
public:
    CICamera(void);
    ~CICamera(void);

    float* GetPositions();
    float* GetDirection();
    float* GetTranslation();
    float* GetRotation();

    //	STRING* GetOriginalId();
    EProjectionType::TYPE GetProjectionType();
    float* GetXFovOrXMag();
    float* GetYFovOrYMag();
    float* GetAspectRatio();
    float* GetNearClippingPlane();
    float* GetFarClippingPlane();

    std::string GetOriginalId();
    std::string GetName();

protected:
    std::vector<float> m_Positions;
    std::vector<float> m_Direction;

    float m_Translation[3];
    float m_Rotation[4];

    std::string m_OriginalId;
    std::string m_Name;
    EProjectionType::TYPE m_ProjectionType;
    float m_XFovOrXMag;
    float m_YFovOrYMag;
    float m_AspectRatio;
    float m_NearClippingPlane;
    float m_FarClippingPlane;
};

#endif //CAMERA_H