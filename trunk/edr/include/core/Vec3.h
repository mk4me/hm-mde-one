#ifndef CVEC_3_H
#define CVEC_3_H

#include <osg/Vec3>

class CVec3: public osg::Vec3f
{
public:
    // Data
 //   float _v[0], _v[1], _v[2];  x, y, z

    // Ctors
    CVec3( float InX, float InY, float InZ ): osg::Vec3f(InX, InY, InZ)
    {
    }
    CVec3( ): osg::Vec3f()
    {
    }
    // Setting
    void SetZero()
    {
        _v[0] = 0;
        _v[1] = 0;
        _v[2] = 0;
    }
    // Operator Overloads
    inline bool operator== (const CVec3& V2) const 
    {
        return (_v[0] == V2._v[0] && _v[1] == V2._v[1] && _v[2] == V2._v[2]);
    }

    inline CVec3 operator+ (const CVec3& V2) const 
    {
        return CVec3( _v[0] + V2._v[0],  _v[1] + V2._v[1],  _v[2] + V2._v[2]);
    }
    inline CVec3 operator- (const CVec3& V2) const
    {
        return CVec3( _v[0] - V2._v[0],  _v[1] - V2._v[1],  _v[2] - V2._v[2]);
    }
    inline CVec3 operator- ( ) const
    {
        return CVec3(-_v[0], -_v[1], -_v[2]);
    }

    inline CVec3 operator/ (float S ) const
    {
        float fInv = 1.0f / S;
        return CVec3 (_v[0] * fInv , _v[1] * fInv, _v[2] * fInv);
    }
    inline CVec3 operator/ (const CVec3& V2) const
    {
        return CVec3 (_v[0] / V2._v[0],  _v[1] / V2._v[1],  _v[2] / V2._v[2]);
    }
    inline CVec3 operator* (const CVec3& V2) const
    {
        return CVec3 (_v[0] * V2._v[0],  _v[1] * V2._v[1],  _v[2] * V2._v[2]);
    }
    inline CVec3 operator* (float S) const
    {
        return CVec3 (_v[0] * S,  _v[1] * S,  _v[2] * S);
    }
    inline void operator += ( const CVec3& V2 ) 
    {
        _v[0] += V2._v[0];
        _v[1] += V2._v[1];
        _v[2] += V2._v[2];
    }

    // Functions
    inline float Dot( const CVec3 &V1 ) const
    {
        return V1._v[0]*_v[0] + V1._v[1]*_v[1] + V1._v[2]*_v[2];
    }

    inline CVec3 CrossProduct( const CVec3 &V2 )  const
    {
        return CVec3(
            _v[1] * V2._v[2]  -  _v[2] * V2._v[1],
            _v[2] * V2._v[0]  -  _v[0] * V2._v[2],
            _v[0] * V2._v[1]  -  _v[1] * V2._v[0] 	);
    }

    // These require math.h for the sqrtf function
    float Magnitude( ) const
    {
        return sqrtf( _v[0]*_v[0] + _v[1]*_v[1] + _v[2]*_v[2] );
    }

    float Distance( const CVec3 &V1 ) const
    {
        return ( *this - V1 ).Magnitude();	
    }

    inline void Normalize()
    {
        float fMag = ( _v[0]*_v[0] + _v[1]*_v[1] + _v[2]*_v[2] );
        if (fMag == 0) {return;}

        float fMult = 1.0f/sqrtf(fMag);            
        _v[0] *= fMult;
        _v[1] *= fMult;
        _v[2] *= fMult;
        return;
    }
};
#endif //CVEC_3_H