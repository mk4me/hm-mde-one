#ifndef LIGHT_H
#define LIGHT_H
#include "dae2Motion.h"
#include "COLLADAFWUniqueId.h"

class IILight;
class CILight;

/**
* Œwiat³o typu reflektor (spot light) - Ÿród³o œwiat³a umieszczone w okreœlonym miejscu
* przestrzeni wysy³a w jednym kierunku œwiat³o w kszta³cie sto¿ka. Intensywnoœæ œwiat³a
* maleje wraz ze wzrostem konta pod jakim œwiat³o jest emitowane. Intensywnoœæ swiat³a maleje
* równie¿ wraz ze wzrostem odleg³oœci od Ÿród³a œwiat³a. Domyœlny wektor kierunku jest ustawiony 
* na [0,0,-1] - œwiat³o jest emitowane w dó³. Aktualna pozycja jest okreœlona w wêŸle node,
* w którym znajduje siê odwo³anie do Ÿród³a œwiat³a o danym identyfikatorze.
* <constant_attenuation>, <linear_attenuation> i <quadratic_attenuation> s¹ u¿ywane do obliczania
* intensywnoœci œwiat³a w podanej odleg³oœci od Ÿród³a:
* A = constant_attenuation + (Dist * linear_attenuation) + (Dist^2 * quadratic_attenuation)
* <falloff_angle> i <falloff_exponent> s¹ u¿ywane do okreœlenia intensywnoœci œwiat³a w zale¿noœci
* od k¹ta pod jakim pada
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
    std::vector<float> m_Diffuse;		// Kolor œwiat³a rozproszonego (r,g,b)
    std::vector<float> m_Specular;		// Kolor œwiat³a odbitego (r,g,b)
    std::vector<float> m_Direction;		// Kierunek promieni (x,y,z)
    float m_Translation[3];				// Przesuniêcie - dla u³atwienia obs³ugi w cube
    float m_Rotation[4];				// Obrót - dla u³atwienia obs³ugi w Cube

    std::string m_OriginalId;				// identyfikator obiektu
    std::string m_Name;						// nazwa obiektu

    ELightType::TYPE m_LightType;				// Rodzaj œwiatla
    float m_SpotOuter;					// Zewnêtrzny promieñ (zasiêg)
    float m_SpotInner;					// Wewnêtrzny promieñ (intensywnoœæ)
    float m_SpotFalloff;				// Wygaszanie (przejœcie miêdzy)
    float m_Range;						// Zasiêg
    float m_AttenuationConst;			// T³umienie - parametr sta³y
    float m_AttenuationLinear;			// T³umienie - parametr liniowy
    float m_AttenuationQuad;			// T³umienie - parametr kwadratowy

    /** Sets the unique id of the object.*/
    void setUniqueId ( const COLLADAFW::UniqueId& uniqueId )
    { 
        mUniqueId = uniqueId; 
    }

private:
    COLLADAFW::UniqueId mUniqueId;
};

#endif //LIGHT_H