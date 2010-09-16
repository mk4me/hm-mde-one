#ifndef CAMERAWRITER_H
#define CAMERAWRITER_H

#include "stdafx.h"

#include "basewriter.h"
#include "Camera.h"

#include "COLLADAFWCamera.h"

class CCameraWriter :
    public CBaseWriter
{
private:
    typedef std::vector<std::string> StringList;
    const COLLADAFW::Camera* mCamera;

    CICamera* m_CurrentCamera;

    /** Disable default copy ctor */
    CCameraWriter(const CCameraWriter& pre);

    /** Disable default assignement operator */
    const CCameraWriter& operator= (const CCameraWriter& pre);

    COLLADABU::Math::Matrix3 mCurrentRotationMatrix;
    COLLADABU::Math::Vector3 mCurrentTranslationVector;

public:
    /** Konstruktor */
    CCameraWriter(CDAE2MotionWriter* cubeITMWriter, const COLLADAFW::Camera* camera);

    /** Destruktor */
    virtual ~CCameraWriter(void);

    bool write(CICamera** ppITMCamera);

};

#endif //CAMERAWRITER_H