/************************************************************************************
*	Files: Quaternioncal.cpp
*
*	Some of the code here was derived from 
*		http://www.neutralzone.org/home/facsys/math.html
*		I acutally retyped most of it to get an understanding of what was going
*		on but the source is pretty similar
*	I would also like to say thanks to the website
*		http://www.flipcode.com/documents/matrfaq.html
*	which helped provide a lot of insight into what was going on
*	
*************************************************************************************/

#include <math.h>
#include <stdio.h>

#include "quaternioncal.h"

#ifndef PI
#define PI (3.1415926535)
#endif

#define FLT_EPSILON 0.005

//--------------------------------------------------------------------------------------------------
Quaternion::Quaternion()
{
    W = 1;
    X = Y = Z = 0;
}

//--------------------------------------------------------------------------------------------------
Quaternion::Quaternion( float w, float x, float y, float z )
{
    W = w;
    X = x;
    Y = y;
    Z = z;
}

//--------------------------------------------------------------------------------------------------
Quaternion::Quaternion( float radX, float radY, float radZ)
{
    fromEulerAngles( radX, radY, radZ );
}

//--------------------------------------------------------------------------------------------------
Quaternion::Quaternion( const Quaternion &tempQuaternion )
{
    W = tempQuaternion.W;
    X = tempQuaternion.X;
    Y = tempQuaternion.Y;
    Z = tempQuaternion.Z;
}

//--------------------------------------------------------------------------------------------------
Quaternion::~Quaternion()
{
}

//--------------------------------------------------------------------------------------------------
void Quaternion::slerp(const Quaternion &QuatVectorA,const Quaternion &QuatVectorB, const float Tele)
{
    float Omega, CosOmega, SinOmega, StandardCaseLinearQuatA, StandardCaseLinearQuatB;

    // Calculate cosine
    CosOmega = QuatVectorA.X*QuatVectorB.X + QuatVectorA.Y*QuatVectorB.Y + QuatVectorA.Z*QuatVectorB.Z + QuatVectorA.W*QuatVectorB.W;

    if ((1.0f+CosOmega) > FLT_EPSILON)
    {
        if ((1.0f-CosOmega) > FLT_EPSILON)
        {
            // standard case SLERP
            Omega = (float)acos(CosOmega);
            SinOmega = (float)sin(Omega);
            StandardCaseLinearQuatA = (float)sin((1.0f-Tele)*Omega) / SinOmega;
            StandardCaseLinearQuatB = (float)sin(Tele*Omega) / SinOmega;
        }
        else
        {
            // a and b are very close so we can do a linear interpolation
            StandardCaseLinearQuatA = 1.0f - Tele;
            StandardCaseLinearQuatB = Tele;
        }
        X = StandardCaseLinearQuatA*QuatVectorA.X + StandardCaseLinearQuatB*QuatVectorB.X;
        Y = StandardCaseLinearQuatA*QuatVectorA.Y + StandardCaseLinearQuatB*QuatVectorB.Y;
        Z = StandardCaseLinearQuatA*QuatVectorA.Z + StandardCaseLinearQuatB*QuatVectorB.Z;
        W = StandardCaseLinearQuatA*QuatVectorA.W + StandardCaseLinearQuatB*QuatVectorB.W;
    }
    else
    {
        X =-QuatVectorB.Y;
        Y = QuatVectorB.X;
        Z =-QuatVectorB.W;
        W = QuatVectorB.Z;
        StandardCaseLinearQuatA = (float)sin((1.0f-Tele) * PI * 0.5);
        StandardCaseLinearQuatB = (float)sin(Tele * PI * 0.5);

        X = StandardCaseLinearQuatA*QuatVectorA.X + StandardCaseLinearQuatB*QuatVectorB.X;
        Y = StandardCaseLinearQuatA*QuatVectorA.Y + StandardCaseLinearQuatB*QuatVectorB.Y;
        Z = StandardCaseLinearQuatA*QuatVectorA.Z + StandardCaseLinearQuatA*QuatVectorB.Z;
        W = StandardCaseLinearQuatA*QuatVectorA.W + StandardCaseLinearQuatA*QuatVectorB.W;
    }
}

//--------------------------------------------------------------------------------------------------
void Quaternion::print()
{
    printf("Quaternion \tW=%f \tX=%f \tY=%f \tZ=%f\n",W,X,Y,Z);
}

//--------------------------------------------------------------------------------------------------
Quaternion operator*(const Quaternion &QuatVectorA, const Quaternion &QuatVectorB)
{
    Quaternion tempQuaternion;

    tempQuaternion.W = QuatVectorA.W*QuatVectorB.W - (QuatVectorA.X*QuatVectorB.X + QuatVectorA.Y*QuatVectorB.Y + QuatVectorA.Z*QuatVectorB.Z);
    tempQuaternion.X = QuatVectorA.W*QuatVectorB.X + QuatVectorB.W*QuatVectorA.X + QuatVectorA.Y*QuatVectorB.Z - QuatVectorA.Z*QuatVectorB.Y;
    tempQuaternion.Y = QuatVectorA.W*QuatVectorB.Y + QuatVectorB.W*QuatVectorA.Y + QuatVectorA.Z*QuatVectorB.X - QuatVectorA.X*QuatVectorB.Z;
    tempQuaternion.Z = QuatVectorA.W*QuatVectorB.Z + QuatVectorB.W*QuatVectorA.Z + QuatVectorA.X*QuatVectorB.Y - QuatVectorA.Y*QuatVectorB.X;

    return tempQuaternion;
}

//--------------------------------------------------------------------------------------------------
const Quaternion& Quaternion::operator*=(const Quaternion &tempQuaternion)
{
    float w = W*tempQuaternion.W - (X*tempQuaternion.X + Y*tempQuaternion.Y + Z*tempQuaternion.Z);

    float x = W*tempQuaternion.X + tempQuaternion.W*X + Y*tempQuaternion.Z - Z*tempQuaternion.Y;
    float y = W*tempQuaternion.Y + tempQuaternion.W*Y + Z*tempQuaternion.X - X*tempQuaternion.Z;
    float z = W*tempQuaternion.Z + tempQuaternion.W*Z + X*tempQuaternion.Y - Y*tempQuaternion.X;

    W = w;
    X = x;
    Y = y;
    Z = z;

    return *this;
}

//--------------------------------------------------------------------------------------------------
void Quaternion::normalize()
{
    float distance = (float)sqrt(W*W + X*X + Y*Y + Z*Z);
    if( distance == 0 )
    {
        W = 1.0;
        X = Y = Z = 0.0;
    }
    else
    {
        W /= distance;
        X /= distance;
        Y /= distance;
        Z /= distance;
    }
}

//--------------------------------------------------------------------------------------------------
void Quaternion::fromAxisAngle( float angle, float x, float y, float z )
{
    float SinAngle = (float)sin( angle / 2 );
    float CosAngle = (float)cos( angle / 2);

    W = CosAngle;
    X = x * SinAngle;
    Y = y * SinAngle;
    Z = z * SinAngle;

    normalize();
}

//--------------------------------------------------------------------------------------------------
void Quaternion::fromEulerAngles( float radX, float radY, float radZ )
{

    Quaternion QuaternionX, QuaternionY, QuaternionZ, QuaternionResult;

    QuaternionX.fromAxisAngle( radX, 1, 0, 0 );
    QuaternionY.fromAxisAngle( radY, 0, 1, 0 );
    QuaternionZ.fromAxisAngle( radZ, 0, 0, 1 );

    QuaternionResult = QuaternionZ * QuaternionX * QuaternionY;

    W = QuaternionResult.W;
    X = QuaternionResult.X;
    Y = QuaternionResult.Y;
    Z = QuaternionResult.Z;
}

//--------------------------------------------------------------------------------------------------
void Quaternion::getAngleAndAxis( float &angle, float &x, float &y, float &z )
{
    angle = (float)(acos( W ) * 2 * 180 / PI);
    float SinAngle = (float)sqrt( 1.0 - W * W );

    if( fabs(SinAngle) < FLT_EPSILON )
        SinAngle = 1;

    x = X / SinAngle;
    y = Y / SinAngle;
    z = Z / SinAngle;
}

// Matrix you get looks like this
//      -----------
//     |0  4  8  12|
// M = |1  5  9  13|
//     |2  6  10 14|
//     |3  7  11 15|
//      ----------- 

//--------------------------------------------------------------------------------------------------
void Quaternion::getRotationMatrix( float* M)
{	
    float xx, xy, xz, wx, yy, yz, wy, zz, wz;

    xx = X * X;
    xy = X * Y;
    xz = X * Z;
    wx = W * X;

    yy = Y * Y;
    yz = Y * Z;
    wy = W * Y;

    zz = Z * Z;
    wz = W * Z;

    M[0]  = 1 - 2 * ( yy + zz );
    M[1]  =     2 * ( xy + wz );
    M[2]  =     2 * ( xz - wy );

    M[4]  =     2 * ( xy - wz );
    M[5]  = 1 - 2 * ( xx + zz );
    M[6]  =     2 * ( yz + wx );

    M[8]  =     2 * ( xz + wy );
    M[9]  =     2 * ( yz - wx );
    M[10] = 1 - 2 * ( xx + yy );

    M[3] = M[7] = M[11] = M[12] = M[13] = M[14] = 0;
    M[15] = 1;
}
