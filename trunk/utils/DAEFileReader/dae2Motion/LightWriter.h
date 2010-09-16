#ifndef LIGHTWRITER_H
#define LIGHTWRITER_H

#include "stdafx.h"

#include "basewriter.h"
#include "Light.h"

#include "COLLADAFWLight.h"

class CLightWriter :
    public CBaseWriter
{
private:
    typedef std::vector<std::string> StringList;
    const COLLADAFW::Light* mLight;

    CILight* m_CurrentLight;

    /** Disable default copy ctor */
    CLightWriter(const CLightWriter& pre);

    /** Disable default assignement operator */
    const CLightWriter& operator= (const CLightWriter& pre);

    COLLADABU::Math::Matrix3 mCurrentRotationMatrix;
    COLLADABU::Math::Vector3 mCurrentTranslationVector;

public:
    /** Konstruktor */
    CLightWriter(CDAE2MotionWriter* cubeITMWriter, const COLLADAFW::Light* light);

    /** Destruktor */
    virtual ~CLightWriter(void);

    bool write(CILight** ppITMLight);

};

#endif //LIGHTWRITER_H