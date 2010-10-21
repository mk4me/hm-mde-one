#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>

#ifndef PI
#define PI (3.1415926535)
#endif

// generic object (class) definition of matrix:
// NOTE: maxsize determines available memory storage, but
// actualsize determines the actual size of the stored matrix in use
// at a particular time.

// Format of this matrix is prepared to OpenGL format (matrix*)
// But we can get matrix[][] using the public method olso.

template <class D> 
class matrix{

    int m_maxsize;  // max number of rows (same as max number of columns)
    int m_actualsize;  // actual size (rows, or columns) of the stored matrix
    D* m_pData;      // where the data contents of the matrix are stored

    void allocate()
    {
        delete[] m_pData;
        m_pData = new D [m_maxsize*m_maxsize];
    };

    matrix() {};                  // private ctor's
    matrix(int newmaxsize) {matrix(newmaxsize,newmaxsize);};

//--------------------------------------------------------------------------------------------------
    void Normalize(double &qx, double &qy, double &qz, double &qw)
    {
        //normalizacja
        float distance = (float)sqrt(qw*qw + qx*qx + qy*qy + qz*qz);
        if( distance == 0 )
        {
            qw = 1.0;
            qx = qy = qz = 0.0;
        }
        else
        {
            qw /= distance;
            qx /= distance;
            qy /= distance;
            qz /= distance;
        }
    }

//--------------------------------------------------------------------------------------------------
    void ParseData2MultiTable(D matrix[][4])
    {
        // Parsowanie danych
        int counter = 0;
        for(int i = 0; i<4; i++)
            for(int j = 0; j<4; j++)
            {
                matrix[i][j] = m_pData[counter];
                counter++;
            }
    }

//--------------------------------------------------------------------------------------------------
public:
    matrix(int newmaxsize, int newactualsize) // the only public ctor
    { 
        if (newmaxsize <= 0) newmaxsize = 5;
        m_maxsize = newmaxsize; 
        if ((newactualsize <= newmaxsize)&&(newactualsize>0))
            m_actualsize = newactualsize;
        else 
            m_actualsize = newmaxsize;
        // since allocate() will first call delete[] on data:
        m_pData = 0;
        allocate();
    };

//--------------------------------------------------------------------------------------------------
    ~matrix() { delete[] m_pData; };


//--------------------------------------------------------------------------------------------------
    void LoadFromEulerAngle(D Yaw, D Pitch, D Roll)
    {
        double qx,qy,qz,qw;

        // zamiana na radiany
        Yaw = Yaw * PI / 180;
        Pitch = Pitch * PI / 180;
        Roll = Roll * PI / 180; 

        Yaw *= 0.5f; Pitch *= 0.5f; Roll *= 0.5f;

        float sy = sinf(Yaw), cy = cosf(Yaw);
        float sp = sinf(Pitch), cp = cosf(Pitch);
        float sr = sinf(Roll), cr = cosf(Roll);

        // Konwersja k¹tów Eulera na kwaternion,
        // który obraca ze wsp. obiektu do œwiata,
        qx = cy*sp*cr + sy*cp*sr;
        qy = sy*cp*cr - cy*sp*sr;
        qz = cy*cp*sr - sy*sp*cr;
        qw = cy*cp*cr + sy*sp*sr;

        Normalize(qx,qy,qz,qw);
        LoadfromQuaternion(qx,qy,qz,qw);
    }

//--------------------------------------------------------------------------------------------------
    void LoadfromQuaternion(double &qx, double &qy, double &qz, double &qw)
    {
        m_maxsize = 4;
        m_actualsize = 4;
        allocate();

        // kwaternion na macierz
        float
            xx = qx * qx, yy = qy * qy, zz = qz * qz,
            xy = qx * qy, xz = qx * qz,
            yz = qy * qz, wx = qw * qx,
            wy = qw * qy, wz = qw * qz;

        m_pData[0] = 1.0f - 2.0f * ( yy + zz ); //11
        m_pData[1] = 2.0f * ( xy + wz ); //12
        m_pData[2] = 2.0f * ( xz - wy ); //13
        m_pData[3] = 0.0f; //14

        m_pData[4] = 2.0f * ( xy - wz ); //21
        m_pData[5] = 1.0f - 2.0f * ( xx + zz ); //22
        m_pData[6] = 2.0f * ( yz + wx ); //23
        m_pData[7] = 0.0f; //24

        m_pData[8] = 2.0f * ( xz + wy ); //31
        m_pData[9] = 2.0f * ( yz - wx ); //32
        m_pData[10] = 1.0f - 2.0f * ( xx + yy ); //33
        m_pData[11] = 0.0f; //34

        m_pData[12] = 0.0f; //41
        m_pData[13] = 0.0f; //42
        m_pData[14] = 0.0f; //43
        m_pData[15] = 1.0f; //44
    }

//--------------------------------------------------------------------------------------------------
    void GetMatrix(D **matrix)
    {

    }

//--------------------------------------------------------------------------------------------------
    void GetMatrix(D *matrix)
    {

    }

//--------------------------------------------------------------------------------------------------
void AddMatrix(matrix& left, matrix& right)
{
    m_actualsize = left.getactualsize();
    if ( m_maxsize < left.getactualsize() )  
    {
        m_maxsize = left.getactualsize();
        allocate();
    }

    for ( int i = 0; i < m_actualsize; i++ )
        for ( int j = 0; j < m_actualsize; j++ )  
        {
            D sum;
            D leftvalue, rightvalue;
            bool success;

            left.getvalue(i,j,leftvalue,success);
            right.getvalue(i,j,rightvalue,success);
            sum = leftvalue + rightvalue;

            setvalue(i,j,sum);
        }

}

// mozna by by³o zrobiæ to na tablicy jednowymiarowej, ale zytelnoœc kodu by os³ab³a wiêc zrobimy konwersje na 
// tablice dwu wymarow¹
//--------------------------------------------------------------------------------------------------
    void GetQuaternion(double &qx, double &qy, double &qz, double &qw)
    {
        if(m_actualsize != 4)
            return;

        D L[4][4];
        ParseData2MultiTable(L);

        float trace = L[0][0] + L[1][1] + L[2][2]; // I removed + 1.0f; see discussion with Ethan
        if( trace > 0 ) {// I changed M_EPSILON to 0
            float s = 0.5f / sqrtf(trace+ 1.0f);
            qw = 0.25f / s;
            qx = ( L[2][1] - L[1][2] ) * s;
            qy = ( L[0][2] - L[2][0] ) * s;
            qz = ( L[1][0] - L[0][1] ) * s;
        } else {
            if ( L[0][0] > L[1][1] && L[0][0] > L[2][2] ) {
                float s = 2.0f * sqrtf( 1.0f + L[0][0] - L[1][1] - L[2][2]);
                qw = (L[2][1] - L[1][2] ) / s;
                qx = 0.25f * s;
                qy = (L[0][1] + L[1][0] ) / s;
                qz = (L[0][2] + L[2][0] ) / s;
            } else if (L[1][1] > L[2][2]) {
                float s = 2.0f * sqrtf( 1.0f + L[1][1] - L[0][0] - L[2][2]);
                qw = (L[0][2] - L[2][0] ) / s;
                qx = (L[0][1] + L[1][0] ) / s;
                qy = 0.25f * s;
                qz = (L[1][2] + L[2][1] ) / s;
            } else {
                float s = 2.0f * sqrtf( 1.0f + L[2][2] - L[0][0] - L[1][1] );
                qw = (L[1][0] - L[0][1] ) / s;
                qx = (L[0][2] + L[2][0] ) / s;
                qy = (L[1][2] + L[2][1] ) / s;
                qz = 0.25f * s;
            }
        }
    }


//--------------------------------------------------------------------------------------------------
    void CompareToIdentity() // w niecie znalaz³em info ze moze sie przydaæ do sprawdzana macierzy - wiec puki co nie zaszkodzi miec coœ podobnego
    {
        int worstdiagonal = 0;
        D maxunitydeviation = 0.0;
        D currentunitydeviation;
        for ( int i = 0; i < m_actualsize; i++ )  {
            currentunitydeviation = m_pData[i*m_maxsize+i] - 1.;
            if ( currentunitydeviation < 0.0) currentunitydeviation *= -1.;
            if ( currentunitydeviation > maxunitydeviation )  {
                maxunitydeviation = currentunitydeviation;
                worstdiagonal = i;
            }
        }
        int worstoffdiagonalrow = 0;
        int worstoffdiagonalcolumn = 0;
        D maxzerodeviation = 0.0;
        D currentzerodeviation ;
        for ( int i = 0; i < m_actualsize; i++ )  {
            for ( int j = 0; j < m_actualsize; j++ )  {
                if ( i == j ) continue;  // we look only at non-diagonal terms
                currentzerodeviation = m_pData[i*m_maxsize+j];
                if ( currentzerodeviation < 0.0) currentzerodeviation *= -1.0;
                if ( currentzerodeviation > maxzerodeviation )  {
                    maxzerodeviation = currentzerodeviation;
                    worstoffdiagonalrow = i;
                    worstoffdiagonalcolumn = j;
                }

            }
        }
        cout << "Worst diagonal value deviation from unity: " 
            << maxunitydeviation << " at row/column " << worstdiagonal << endl;
        cout << "Worst off-diagonal value deviation from zero: " 
            << maxzerodeviation << " at row = " << worstoffdiagonalrow 
            << ", column = " << worstoffdiagonalcolumn << endl;
    }

//--------------------------------------------------------------------------------------------------
    void SetToProduct(matrix& left, matrix& right) // mnozenie macierzy
    {
        m_actualsize = left.getactualsize();
        if ( m_maxsize < left.getactualsize() )  
        {
            m_maxsize = left.getactualsize();
            allocate();
        }

        for ( int i = 0; i < m_actualsize; i++ )
            for ( int j = 0; j < m_actualsize; j++ )  
            {
                D sum = 0.0;
                D leftvalue, rightvalue;
                bool success;
                for (int c = 0; c < m_actualsize; c++) 
                {
                    left.getvalue(i,c,leftvalue,success);
                    right.getvalue(c,j,rightvalue,success);
                    sum += leftvalue * rightvalue;
                }
                setvalue(i,j,sum);
            }

    // Mnozenie macierzy  alternarywna wersja dla macierzy matrix[][]
    //         int i, j, k;
    //         for(i = 0; i < 4; i++)
    //             for(j = 0; j < 4; j++)
    //                 L[i][j] = 0;
    //         for(i = 0; i < 4; i++) //ILWIERSZY
    //             for(j = 0; j < 4; j++) //ILWIERSZY
    //                 for(k = 0; k < 4; k++) //ILKOLUMN
    //                     L[i][j] = L[i][j] + B[i][k] * M[k][j];

    }

//--------------------------------------------------------------------------------------------------
    void copymatrix(matrix&  source) // kopiowanie macierzy
    {
        m_actualsize = source.getactualsize();
        if ( m_maxsize < source.getactualsize() )  
        {
            m_maxsize = source.getactualsize();
            allocate();
        }
        for ( int i = 0; i < m_actualsize; i++ )
            for ( int j = 0; j < m_actualsize; j++ ) 
            {
                D value;
                bool success;
                source.getvalue(i,j,value,success);
                m_pData[i*m_maxsize+j] = value;
            }
    };

//--------------------------------------------------------------------------------------------------
    void setactualsize(int newactualsize) // uzupe³nianie wielkoœci macierzy
    {
        if ( newactualsize > m_maxsize )
        {
            m_maxsize = newactualsize ; // * 2;  // wastes memory but saves
            // time otherwise required for
            // operation new[]
            allocate();
        }
        if (newactualsize >= 0) 
            m_actualsize = newactualsize;
    };

//--------------------------------------------------------------------------------------------------
    int getactualsize() 
    { 
        return m_actualsize; 
    };

//--------------------------------------------------------------------------------------------------
    void getvalue(int row, int column, D& returnvalue, bool& success) 
    {
        if ( (row>=m_maxsize) || (column>=m_maxsize) 
            || (row<0) || (column<0) )
        {  success = false;
        return;    }
        returnvalue = m_pData[ row * m_maxsize + column ];
        success = true;
    };

//--------------------------------------------------------------------------------------------------
    bool setvalue(int row, int column, D newvalue)
    {
        if ( (row >= m_maxsize) || (column >= m_maxsize) 
            || (row<0) || (column<0) ) return false;
        m_pData[ row * m_maxsize + column ] = newvalue;
        return true;
    };

//--------------------------------------------------------------------------------------------------
    void invert()
    {
        if (m_actualsize <= 0) return;  // sanity check
        if (m_actualsize == 1) return;  // must be of dimension >= 2
        for (int i=1; i < m_actualsize; i++) m_pData[i] /= m_pData[0]; // normalize row 0
        for (int i=1; i < m_actualsize; i++)  { 
            for (int j=i; j < m_actualsize; j++)  { // do a column of L
                D sum = 0.0;
                for (int k = 0; k < i; k++)  
                    sum += m_pData[j*m_maxsize+k] * m_pData[k*m_maxsize+i];
                m_pData[j*m_maxsize+i] -= sum;
            }
            if (i == m_actualsize-1) continue;
            for (int j=i+1; j < m_actualsize; j++)  {  // do a row of U
                D sum = 0.0;
                for (int k = 0; k < i; k++)
                    sum += m_pData[i*m_maxsize+k]*m_pData[k*m_maxsize+j];
                m_pData[i*m_maxsize+j] = 
                    (m_pData[i*m_maxsize+j]-sum) / m_pData[i*m_maxsize+i];
            }
        }
        for ( int i = 0; i < m_actualsize; i++ )  // invert L
            for ( int j = i; j < m_actualsize; j++ )  {
                D x = 1.0;
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
                    D sum = 0.0;
                    for ( int k = i; k < j; k++ )
                        sum += m_pData[k*m_maxsize+j]*( (i==k) ? 1.0 : m_pData[i*m_maxsize+k] );
                    m_pData[i*m_maxsize+j] = -sum;
                }
                for ( int i = 0; i < m_actualsize; i++ )   // final inversion
                    for ( int j = 0; j < m_actualsize; j++ )  {
                        D sum = 0.0;
                        for ( int k = ((i>j)?i:j); k < m_actualsize; k++ )  
                            sum += ((j==k)?1.0:m_pData[j*m_maxsize+k])*m_pData[k*m_maxsize+i];
                        m_pData[j*m_maxsize+i] = sum;
                    }
    };
};
#endif // MATRIX_H

