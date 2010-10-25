#ifndef TRANSFORM_H
#define TRANSFORM_H

class Matrix
{

};

void CopyMatrix(double source[][4], double destinity[][4]);
void SetToProduct(double left[4][4], double right[4][4], double out[4][4]);
void Invert(double source[][4], int m_actualsize = 4, int m_maxsize = 4);
void LoadFromTranslationVec(float X, float Y, float Z, double m_pData[4][4]);

void matrix_transpose(double a[4][4], double b[4][4]);
void matrix_print(char *str, double a[4][4]);
void matrix_transform_affine(double m[4][4], double x, double y, double z, float pt[3]);
void matrix_mult(double a[][4], double b[][4], double c[][4]);

void v3_cross(float a[3], float b[3], float c[3]);
float v3_mag(float a[3]);
float v3_dot(float a[3], float b[3]);

//Rotate vector v aroud axis X by angle a, around axis Y by angle b and around axis Z by angle c
void vector_rotationXYZ(float *v, float a, float b, float c);

//Create Rotation matrix, that rotates around axis X by angle a
void rotationX(double r[][4], float a);
//Create Rotation matrix, that rotates around axis Y by angle a
void rotationY(double r[][4], float a);
//Create Rotation matrix, that rotates around axis Z by angle a
void rotationZ(double r[][4], float a);

//Return the angle between vectors v1 and v2 around the given axis 
float GetAngle(float* v1, float* v2, float* axis);

#endif
