// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the DAE2MOTION_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// DAE2MOTION_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#ifndef DEA2MOTON_H
#define DEA2MOTON_H

#ifdef DAE2MOTION_EXPORTS
#define DAE2MOTION_API __declspec(dllexport)
#else
#define DAE2MOTION_API __declspec(dllimport)
#endif

class IISceneGraph;
class IISceneGraphNode;
class IIBaseObject;
class IIMesh;
class IISubMesh;
class IICamera;
class IILight;

// Typ obiektu
namespace ECubeITMObjectType
{
    enum TYPE
    {
        UNKNOWN = 0,
        MESH,
        SUBMESH,
        LIGHT,
        CAMERA,

        MAXVALUE
    };
}

// Typ oœwietlenia
namespace ELightType
{
    enum TYPE
    {
        PointLight = 0,
        DirectionalLight,
        SpotLight,
        AmbientLight
    };
};

// Typ kamery
namespace EProjectionType
{
    enum TYPE
    {
        Orthographic = 0,
        Perspective
    };
};

class DAE2MOTION_API IISceneGraph
{

};

class DAE2MOTION_API IISceneGraphNode
{

};

class DAE2MOTION_API IIBaseObject
{
    //	virtual STRING* GetOriginalId() = 0;
};

class DAE2MOTION_API IISubMesh: public IIBaseObject
{
public:
    // Format wierzcho³ka
    virtual bool HasNormals() = 0;
    virtual bool HasTangents() = 0 ;
    virtual bool HasBinormals() = 0 ;
    virtual bool HasColors() = 0 ;
    virtual bool HasUV0Coords() = 0 ;
    virtual bool HasUV1Coords() = 0 ;

    // Rozmiar buforów
    virtual unsigned int GetVertexCount() = 0;

    // Zwraca wskaŸnik na pierwszy element bufora
    virtual float* GetPositions() = 0;
    virtual float* GetNormals() = 0;
    virtual float* GetTangents() = 0;
    virtual float* GetBinormals() = 0;
    virtual float* GetColors() = 0;
    virtual float* GetUV0Coords() = 0;
    virtual float* GetUV1Coords() = 0;

    virtual unsigned int GetIndicesCount() = 0;
    virtual unsigned long* GetIndices() = 0;
};


class DAE2MOTION_API IIMesh: public IIBaseObject
{
public:    
    virtual IISubMesh* GetSubMesh(unsigned int subMeshID) = 0;
    virtual unsigned int GetSubMeshCount() = 0;
    virtual float* GetTranslation() = 0;
    virtual float* GetRotation() = 0;
};

class DAE2MOTION_API IICamera: public IIBaseObject
{
public:
    virtual float* GetPositions() = 0;
    virtual float* GetDirection() = 0;
    virtual float* GetTranslation() = 0;
    virtual float* GetRotation() = 0;

    virtual EProjectionType::TYPE GetProjectionType() = 0;
    virtual float* GetXFovOrXMag() = 0;
    virtual float* GetYFovOrYMag() = 0;
    virtual float* GetAspectRatio() = 0;
    virtual float* GetNearClippingPlane() = 0;
    virtual float* GetFarClippingPlane() = 0;

    virtual std::string GetOriginalId() = 0;
    virtual std::string GetName() = 0;

};

class DAE2MOTION_API IILight: public IIBaseObject
{
public:
    // Zwraca wskaŸnik na pierwszy element bufora
    virtual float* GetPositions() = 0;
    virtual float* GetDiffuse() = 0;
    virtual float* GetTranslation() = 0;
    virtual float* GetRotation() = 0;
    virtual float* GetSpecular() = 0;
    virtual float* GetDirection() = 0;

    virtual ELightType::TYPE GetLightType() = 0;
    virtual float* GetSpotlightInnerAngle() = 0;
    virtual float* GetSpotlightOuterAngle() = 0;
    virtual float* GetSpotlightFalloff() = 0; 

    virtual float* GetAttenuationConstant() = 0;
    virtual float* GetAttenuationLinear() = 0;
    virtual float* GetAttenuationQuadric() = 0;
    virtual float* GetAttenuationRange() = 0;

    virtual std::string GetOriginalId() = 0;
    virtual std::string GetName() = 0;
    //virtual VOID setAttenuation(FLOAT range, FLOAT constant, FLOAT linear, FLOAT quadratic) = 0;
};


// Strutura wejsciowo wyjsciowa biblioteki
struct SDea2Motion 
{
    // Dane wejœciowe
    std::string in_fileName;     // nazwa pliku, tylko informacyjnie
    char*  in_buffer;       // Bufor wejœciowy
    unsigned int   in_length;       // D³ugoœæ bufora wejœciowego

    // Dane wyjœciowe
    std::vector<IIMesh*>   out_meshes;     // wektor siatek
    std::vector<IILight*>  out_lights;     // wektor Ÿróde³ swiat³a
    std::vector<IICamera*> out_cameras;    // wektor kamer
};

// This class is exported from the dae2Motion.dll
class DAE2MOTION_API Cdae2Motion {
public:
	Cdae2Motion(void);
	// TODO: add your methods here.
};

extern DAE2MOTION_API int ndae2Motion;

DAE2MOTION_API bool Convert2Motion(SDea2Motion* data);


#endif // DEA2MOTON_H