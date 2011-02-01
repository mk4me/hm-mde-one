#ifndef LIGHT_H
#define LIGHT_H
#include "dae2Motion.h"
#include "COLLADAFWUniqueId.h"

class IILight;
class CILight;

/**
* �wiat�o typu reflektor (spot light) - �r�d�o �wiat�a umieszczone w okre�lonym miejscu
* przestrzeni wysy�a w jednym kierunku �wiat�o w kszta�cie sto�ka. Intensywno�� �wiat�a
* maleje wraz ze wzrostem konta pod jakim �wiat�o jest emitowane. Intensywno�� swiat�a maleje
* r�wnie� wraz ze wzrostem odleg�o�ci od �r�d�a �wiat�a. Domy�lny wektor kierunku jest ustawiony 
* na [0,0,-1] - �wiat�o jest emitowane w d�. Aktualna pozycja jest okre�lona w w�le node,
* w kt�rym znajduje si� odwo�anie do �r�d�a �wiat�a o danym identyfikatorze.
* <constant_attenuation>, <linear_attenuation> i <quadratic_attenuation> s� u�ywane do obliczania
* intensywno�ci �wiat�a w podanej odleg�o�ci od �r�d�a:
* A = constant_attenuation + (Dist * linear_attenuation) + (Dist^2 * quadratic_attenuation)
* <falloff_angle> i <falloff_exponent> s� u�ywane do okre�lenia intensywno�ci �wiat�a w zale�no�ci
* od k�ta pod jakim pada
*/

class CILight: public IILight
{
    friend class CLightWriter;

public:
    // Konstruktor i destruktor
    CILight(void);
    ~CILight(void);

    std::string GetOriginalId();
    std::string GetName();
    const COLLADAFW::UniqueId& getUniqueId() const { return mUniqueId; }

    float* GetPositions();
    float* GetDiffuse();
    float* GetTranslation();
    float* GetRotation();
    float* GetSpecular();
    float* GetDirection();

    ELightType::TYPE GetLightType();
    float* GetSpotlightInnerAngle();
    float* GetSpotlightOuterAngle();
    float* GetSpotlightFalloff(); 

    float* GetAttenuationConstant();
    float* GetAttenuationLinear();
    float* GetAttenuationQuadric();
    float* GetAttenuationRange();

protected:
    std::vector<float> m_Positions;		// Pozycja (x,y,z)
    std::vector<float> m_Diffuse;		// Kolor �wiat�a rozproszonego (r,g,b)
    std::vector<float> m_Specular;		// Kolor �wiat�a odbitego (r,g,b)
    std::vector<float> m_Direction;		// Kierunek promieni (x,y,z)
    float m_Translation[3];				// Przesuni�cie - dla u�atwienia obs�ugi w cube
    float m_Rotation[4];				// Obr�t - dla u�atwienia obs�ugi w Cube

    std::string m_OriginalId;				// identyfikator obiektu
    std::string m_Name;						// nazwa obiektu

    ELightType::TYPE m_LightType;				// Rodzaj �wiatla
    float m_SpotOuter;					// Zewn�trzny promie� (zasi�g)
    float m_SpotInner;					// Wewn�trzny promie� (intensywno��)
    float m_SpotFalloff;				// Wygaszanie (przej�cie mi�dzy)
    float m_Range;						// Zasi�g
    float m_AttenuationConst;			// T�umienie - parametr sta�y
    float m_AttenuationLinear;			// T�umienie - parametr liniowy
    float m_AttenuationQuad;			// T�umienie - parametr kwadratowy

    /** Sets the unique id of the object.*/
    void setUniqueId ( const COLLADAFW::UniqueId& uniqueId )
    { 
        mUniqueId = uniqueId; 
    }

private:
    COLLADAFW::UniqueId mUniqueId;
};

#endif //LIGHT_H