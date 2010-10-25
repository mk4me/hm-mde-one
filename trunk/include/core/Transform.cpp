#include <cmath>
#include <cstdio>
#include "Transform.h"

#ifndef M_PI
#define M_PI 3.14159265
#endif


void CopyMatrix(double source[][4], double destinity[][4]) // kopiowanie macierzy
{

	for ( int i = 0; i < 4; i++ )
		for ( int j = 0; j < 4; j++ ) 
		{
			destinity[i][j] = source[i][j];
		}
};

void SetToProduct(double left[4][4], double right[4][4], double out[4][4]) // mnozenie macierzy
{
		// Mnozenie macierzy  alternarywna wersja dla macierzy matrix[][]
		         int i, j, k;
		         for(i = 0; i < 4; i++)
		             for(j = 0; j < 4; j++)
		                 out[i][j] = 0;
		         for(i = 0; i < 4; i++) //ILWIERSZY
		             for(j = 0; j < 4; j++) //ILWIERSZY
		                 for(k = 0; k < 4; k++) //ILKOLUMN
		                     out[i][j] = out[i][j] + left[i][k] * right[k][j];
}

void Invert(double source[][4], int m_actualsize, int m_maxsize)
{

	double *m_pData = new double[16];

	m_pData[0] = source[0][0]; //11
	m_pData[1] = source[0][1]; //12
	m_pData[2] = source[0][2]; //13
	m_pData[3] = source[0][3]; //14

	m_pData[4] = source[1][0]; //21
	m_pData[5] = source[1][1]; //22
	m_pData[6] = source[1][2]; //23
	m_pData[7] = source[1][3]; //24

	m_pData[8] = source[2][0]; //31
	m_pData[9] = source[2][1]; //32
	m_pData[10] = source[2][2]; //33
	m_pData[11] = source[2][3]; //34

	m_pData[12] = source[3][0]; //41
	m_pData[13] = source[3][1]; //42
	m_pData[14] = source[3][2]; //43
	m_pData[15] = source[3][3]; //44

	for (int i=1; i < m_actualsize; i++) m_pData[i] /= m_pData[0]; // normalize row 0
	for (int i=1; i < m_actualsize; i++)  { 
		for (int j=i; j < m_actualsize; j++)  { // do a column of L
			double sum = 0.0;
			for (int k = 0; k < i; k++)  
				sum += m_pData[j*m_maxsize+k] * m_pData[k*m_maxsize+i];
			m_pData[j*m_maxsize+i] -= sum;
		}
		if (i == m_actualsize-1) continue;
		for (int j=i+1; j < m_actualsize; j++)  {  // do a row of U
			double sum = 0.0;
			for (int k = 0; k < i; k++)
				sum += m_pData[i*m_maxsize+k]*m_pData[k*m_maxsize+j];
			m_pData[i*m_maxsize+j] = 
				(m_pData[i*m_maxsize+j]-sum) / m_pData[i*m_maxsize+i];
		}
	}
	for ( int i = 0; i < m_actualsize; i++ )  // invert L
		for ( int j = i; j < m_actualsize; j++ )  {
			double x = 1.0;
			if ( i != j ) {
				x = 0.0;
				for ( int k = i; k < j; k++ ) 
					x -= m_pData[j*m_maxsize+k]*m_pData[k*m_maxsize+i];
			}
			m_pData[j*m_maxsize+i] = x / m_pData[j*m_maxsize+j];
		}
		for ( int i = 0; i < m_actualsize; i++ )   // invert U
			for ( int j = i; j < m_actualsize; j++ )  {
				if ( i == j ) continue;
				double sum = 0.0;
				for ( int k = i; k < j; k++ )
					sum += m_pData[k*m_maxsize+j]*( (i==k) ? 1.0 : m_pData[i*m_maxsize+k] );
				m_pData[i*m_maxsize+j] = -sum;
			}
			for ( int i = 0; i < m_actualsize; i++ )   // final inversion
				for ( int j = 0; j < m_actualsize; j++ )  {
					double sum = 0.0;
					for ( int k = ((i>j)?i:j); k < m_actualsize; k++ )  
						sum += ((j==k)?1.0:m_pData[j*m_maxsize+k])*m_pData[k*m_maxsize+i];
					m_pData[j*m_maxsize+i] = sum;
				}

				 source[0][0] = m_pData[0]; //11
				 source[0][1] = m_pData[1]; //12
				 source[0][2] = m_pData[2]; //13
				 source[0][3] = m_pData[3]; //14

				 source[1][0] = m_pData[4]; //21
				 source[1][1] = m_pData[5]; //22
				 source[1][2] = m_pData[6]; //23
				 source[1][3] = m_pData[7]; //24

				 source[2][0] = m_pData[8]; //31
				 source[2][1] = m_pData[9]; //32
				 source[2][2] = m_pData[10]; //33
				 source[2][3] = m_pData[11]; //34

				 source[3][0] = m_pData[12]; //41
				 source[3][1] = m_pData[13]; //42
				 source[3][2] = m_pData[14]; //43
				 source[3][3] = m_pData[15]; //44
};

void LoadFromTranslationVec(float X, float Y, float Z, double m_pData[4][4])
{
	m_pData[0][0]=1;      m_pData[0][1]=0;       m_pData[0][2]=0; m_pData[0][3]=X;
	m_pData[1][0]=0;	  m_pData[1][1]=1;       m_pData[1][2]=0; m_pData[1][3]=Y;
	m_pData[2][0]=0;      m_pData[2][1]=0;       m_pData[2][2]=1; m_pData[2][3]=Z;
	m_pData[3][0]=0;      m_pData[3][1]=0;       m_pData[3][2]=0; m_pData[3][3]=1;
}

/* Compute transpose of a matrix
Input: matrix  a
Output: matrix b = Transpose(a)
*/
//--------------------------------------------------------------------------------------------------
void matrix_transpose(double a[4][4], double b[4][4]) 
{
    int i, j;

    for (i=0; i<4; i++)
        for (j=0; j<4; j++)
            b[i][j] = a[j][i];
}

/* Print the matrix
Input:	
*/
//--------------------------------------------------------------------------------------------------
void matrix_print(char *str, double a[4][4]) 
{
    int i;

    printf("matrix %s:\n", str);
    for (i=0; i<4; i++)
        printf(" %8.3f %8.3f %8.3f %8.3f\n",
        a[i][0], a[i][1], a[i][2], a[i][3]);
}


/* Transform the point (x,y,z) by the matrix m, which is
assumed to be affine (last row 0 0 0 1) 
this is just a matrix-vector multiply 
*/
//--------------------------------------------------------------------------------------------------
void matrix_transform_affine(double m[4][4],
                             double x, double y, 
                             double z, float pt[3]) 
{
    pt[0] = m[0][0]*x + m[0][1]*y + m[0][2]*z + m[0][3];
    pt[1] = m[1][0]*x + m[1][1]*y + m[1][2]*z + m[1][3];
    pt[2] = m[2][0]*x + m[2][1]*y + m[2][2]*z + m[2][3];
}




//--------------------------------------------------------------------------------------------------
void v3_cross(float a[3], float b[3], float c[3]) 
{
    /* cross product of two vectors: c = a x b */
    c[0] = a[1]*b[2]-a[2]*b[1];
    c[1] = a[2]*b[0]-a[0]*b[2];
    c[2] = a[0]*b[1]-a[1]*b[0];
}

//--------------------------------------------------------------------------------------------------
float v3_dot(float a[3], float b[3])
{
    return(a[0]*b[0]+a[1]*b[1]+a[2]*b[2]);
}

//--------------------------------------------------------------------------------------------------
float v3_mag(float a[3])
{
    return(sqrt(a[0]*a[0]+a[1]*a[1]+a[2]*a[2])); 
}

//--------------------------------------------------------------------------------------------------
void rotationZ(double r[][4], float a)
{
    a=a*M_PI/180.;
    r[0][0]=cos(a); r[0][1]=-sin(a); r[0][2]=0; r[0][3]=0;
    r[1][0]=sin(a); r[1][1]=cos(a);  r[1][2]=0; r[1][3]=0;
    r[2][0]=0;      r[2][1]=0;       r[2][2]=1; r[2][3]=0;
    r[3][0]=0;      r[3][1]=0;       r[3][2]=0; r[3][3]=1;
}

//--------------------------------------------------------------------------------------------------
void rotationY(double r[][4], float a)
{
    a=a*M_PI/180.;
    r[0][0]=cos(a);  r[0][1]=0;       r[0][2]=sin(a); r[0][3]=0;
    r[1][0]=0;       r[1][1]=1;       r[1][2]=0;      r[1][3]=0;
    r[2][0]=-sin(a); r[2][1]=0;       r[2][2]=cos(a); r[2][3]=0;
    r[3][0]=0;       r[3][1]=0;       r[3][2]=0;      r[3][3]=1;
}

//--------------------------------------------------------------------------------------------------
void rotationX(double r[][4], float a)
{
    a=a*M_PI/180.;
    r[0][0]=1;       r[0][1]=0;       r[0][2]=0;       r[0][3]=0;
    r[1][0]=0;       r[1][1]=cos(a);  r[1][2]=-sin(a); r[1][3]=0;
    r[2][0]=0;       r[2][1]=sin(a);  r[2][2]=cos(a);  r[2][3]=0;
    r[3][0]=0;       r[3][1]=0;       r[3][2]=0;       r[3][3]=1;
}

//--------------------------------------------------------------------------------------------------
void matrix_mult(double a[][4], double b[][4], double c[][4])
{
    int i, j, k;
    for(i=0;i<4;i++)
        for(j=0;j<4;j++)
        {
            c[i][j]=0;
            for(k=0;k<4;k++)
                c[i][j]+=a[i][k]*b[k][j];
        }
}

/*
Rotate vector v by a, b, c in X,Y,Z order.
v_out = Rz(c)*Ry(b)*Rx(a)*v_in
*/
//--------------------------------------------------------------------------------------------------
void vector_rotationXYZ(float *v, float a, float b, float c)
{
    float x, y, z;
    double Rx[4][4], Ry[4][4], Rz[4][4];

    //Rz is a rotation matrix about Z axis by angle c, same for Ry and Rx
    rotationZ(Rz, c);
    rotationY(Ry, b);
    rotationX(Rx, a);

    //Matrix vector multiplication to generate the output vector v.
    matrix_transform_affine(Rz, v[0], v[1], v[2], v);
    matrix_transform_affine(Ry, v[0], v[1], v[2], v);
    matrix_transform_affine(Rx, v[0], v[1], v[2], v);
}

//get the angle from vector v1 to vector v2 around the axis
//--------------------------------------------------------------------------------------------------
float GetAngle(float* v1, float* v2, float* axis)
{
    float dot_prod = v3_dot(v1, v2);
    float r_axis_len = v3_mag(axis);

    float theta = atan2(r_axis_len, dot_prod);


    return theta;


    //DEBUG: Fix this
    //	float sin_theta = 

#if 0
    //check if dot_prod > 0
    if (fabsf(dot_prod) > EPS)
        theta = atan2(r_axis_len, dot_prod);
    //the dot_prod = 0
    else
    {
        //
        theta = 
    }
#endif

#if 0
    static float z_dir[3]={0., 0., 1.};
    float r_axis[3], mag, theta;
    r_axis = {
        float mag = v3_mag(r_axis);
        if(mag > 1.) mag=1;         // hack to prevent error in asin() calculation caused by
        if(mag < -1.) mag=-1;       // the insufficient machine precision of floating point calculation
        theta=asin(mag);
        if(v3_dot(z_dir, pBone->dir) < 0)
        {
            if(mag > 0) theta = M_PI - theta;
            else theta = - M_PI - theta;
        }
        return theta;
#endif

#if 0	
        float mag = v3_mag(axis);
        if(mag > 1.) mag=1;         // hack to prevent error in asin() calculation caused by
        if(mag < -1.) mag=-1;       // the insufficient machine precision of floating point calculation
        float theta=asin(mag);
        if(v3_dot(v1, v2) < 0)
        {
            if(mag > 0) theta = M_PI - theta;
            else theta = - M_PI - theta;
        }
        return theta;
#endif
    }