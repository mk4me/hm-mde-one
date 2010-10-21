#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>

#ifndef PI
#define PI (3.1415926535)
#endif

// generic object (class) definition of matrix:
template <class D> class matrix{
    // NOTE: maxsize determines available memory storage, but
    // actualsize determines the actual size of the stored matrix in use
    // at a particular time.

    // Format of this matrix is prepared to OpenGL format (matrix*)
    // But we can get matrix[][] using the public method olso.


    int maxsize;  // max number of rows (same as max number of columns)
    int actualsize;  // actual size (rows, or columns) of the stored matrix
    D* data;      // where the data contents of the matrix are stored

    void allocate()
    {
        delete[] data;
        data = new D [maxsize*maxsize];
    };

    matrix() {};                  // private ctor's
    matrix(int newmaxsize) {matrix(newmaxsize,newmaxsize);};

//--------------------------------------------------------------------------------------------------
    void normalize(double &qx, double &qy, double &qz, double &qw)
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
    void parseData2MultiTable(D matrix[][4])
    {
        // Parsowanie danych
        int counter = 0;
        for(int i = 0; i<4; i++)
            for(int j = 0; j<4; j++)
            {
                matrix[i][j] = data[counter];
                counter++;
            }
    }

//--------------------------------------------------------------------------------------------------
public:
    matrix(int newmaxsize, int newactualsize) // the only public ctor
    { 
        if (newmaxsize <= 0) newmaxsize = 5;
        maxsize = newmaxsize; 
        if ((newactualsize <= newmaxsize)&&(newactualsize>0))
            actualsize = newactualsize;
        else 
            actualsize = newmaxsize;
        // since allocate() will first call delete[] on data:
        data = 0;
        allocate();
    };

//--------------------------------------------------------------------------------------------------
    ~matrix() { delete[] data; };


//--------------------------------------------------------------------------------------------------
    void loadFromEulerAngle(D Yaw, D Pitch, D Roll)
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

        normalize(qx,qy,qz,qw);
        loadfromQuaternion(qx,qy,qz,qw);
    }

//--------------------------------------------------------------------------------------------------
    void loadfromQuaternion(double &qx, double &qy, double &qz, double &qw)
    {
        maxsize = 4;
        actualsize = 4;
        allocate();

        // kwaternion na macierz
        float
            xx = qx * qx, yy = qy * qy, zz = qz * qz,
            xy = qx * qy, xz = qx * qz,
            yz = qy * qz, wx = qw * qx,
            wy = qw * qy, wz = qw * qz;

        data[0] = 1.0f - 2.0f * ( yy + zz ); //11
        data[1] = 2.0f * ( xy + wz ); //12
        data[2] = 2.0f * ( xz - wy ); //13
        data[3] = 0.0f; //14

        data[4] = 2.0f * ( xy - wz ); //21
        data[5] = 1.0f - 2.0f * ( xx + zz ); //22
        data[6] = 2.0f * ( yz + wx ); //23
        data[7] = 0.0f; //24

        data[8] = 2.0f * ( xz + wy ); //31
        data[9] = 2.0f * ( yz - wx ); //32
        data[10] = 1.0f - 2.0f * ( xx + yy ); //33
        data[11] = 0.0f; //34

        data[12] = 0.0f; //41
        data[13] = 0.0f; //42
        data[14] = 0.0f; //43
        data[15] = 1.0f; //44
    }

//--------------------------------------------------------------------------------------------------
    void getMatrix(D **matrix)
    {

    }

//--------------------------------------------------------------------------------------------------
    void getMatrix(D *matrix)
    {

    }

//--------------------------------------------------------------------------------------------------
void addMatrix(matrix& left, matrix& right)
{
    actualsize = left.getactualsize();
    if ( maxsize < left.getactualsize() )  
    {
        maxsize = left.getactualsize();
        allocate();
    }

    for ( int i = 0; i < actualsize; i++ )
        for ( int j = 0; j < actualsize; j++ )  
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

// mozn by by³o zrobiæ to na tablicy jednowymiarowej, ale zytelnoœc kodu by os³ab³a wiêc zrobimy konwersje na 
// tablice dwu wymarow¹
//--------------------------------------------------------------------------------------------------
    void getQuaternion(double &qx, double &qy, double &qz, double &qw)
    {
        if(actualsize != 4)
            return;

        D L[4][4];
        parseData2MultiTable(L);

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
    void comparetoidentity() // w niecie znalaz³em info ze moze sie przydaæ do sprawdzana macierzy - wiec puki co nie zaszkodzi miec coœ podobnego
    {
        int worstdiagonal = 0;
        D maxunitydeviation = 0.0;
        D currentunitydeviation;
        for ( int i = 0; i < actualsize; i++ )  {
            currentunitydeviation = data[i*maxsize+i] - 1.;
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
        for ( int i = 0; i < actualsize; i++ )  {
            for ( int j = 0; j < actualsize; j++ )  {
                if ( i == j ) continue;  // we look only at non-diagonal terms
                currentzerodeviation = data[i*maxsize+j];
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
    void settoproduct(matrix& left, matrix& right) // mnozenie macierzy
    {
        actualsize = left.getactualsize();
        if ( maxsize < left.getactualsize() )  
        {
            maxsize = left.getactualsize();
            allocate();
        }

        for ( int i = 0; i < actualsize; i++ )
            for ( int j = 0; j < actualsize; j++ )  
            {
                D sum = 0.0;
                D leftvalue, rightvalue;
                bool success;
                for (int c = 0; c < actualsize; c++) 
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
        actualsize = source.getactualsize();
        if ( maxsize < source.getactualsize() )  
        {
            maxsize = source.getactualsize();
            allocate();
        }
        for ( int i = 0; i < actualsize; i++ )
            for ( int j = 0; j < actualsize; j++ ) 
            {
                D value;
                bool success;
                source.getvalue(i,j,value,success);
                data[i*maxsize+j] = value;
            }
    };

//--------------------------------------------------------------------------------------------------
    void setactualsize(int newactualsize) // uzupe³nianie wielkoœci macierzy
    {
        if ( newactualsize > maxsize )
        {
            maxsize = newactualsize ; // * 2;  // wastes memory but saves
            // time otherwise required for
            // operation new[]
            allocate();
        }
        if (newactualsize >= 0) 
            actualsize = newactualsize;
    };

//--------------------------------------------------------------------------------------------------
    int getactualsize() { return actualsize; };

//--------------------------------------------------------------------------------------------------
    void getvalue(int row, int column, D& returnvalue, bool& success) 
    {
        if ( (row>=maxsize) || (column>=maxsize) 
            || (row<0) || (column<0) )
        {  success = false;
        return;    }
        returnvalue = data[ row * maxsize + column ];
        success = true;
    };

//--------------------------------------------------------------------------------------------------
    bool setvalue(int row, int column, D newvalue)
    {
        if ( (row >= maxsize) || (column >= maxsize) 
            || (row<0) || (column<0) ) return false;
        data[ row * maxsize + column ] = newvalue;
        return true;
    };

//--------------------------------------------------------------------------------------------------
    void invert()
    {
        if (actualsize <= 0) return;  // sanity check
        if (actualsize == 1) return;  // must be of dimension >= 2
        for (int i=1; i < actualsize; i++) data[i] /= data[0]; // normalize row 0
        for (int i=1; i < actualsize; i++)  { 
            for (int j=i; j < actualsize; j++)  { // do a column of L
                D sum = 0.0;
                for (int k = 0; k < i; k++)  
                    sum += data[j*maxsize+k] * data[k*maxsize+i];
                data[j*maxsize+i] -= sum;
            }
            if (i == actualsize-1) continue;
            for (int j=i+1; j < actualsize; j++)  {  // do a row of U
                D sum = 0.0;
                for (int k = 0; k < i; k++)
                    sum += data[i*maxsize+k]*data[k*maxsize+j];
                data[i*maxsize+j] = 
                    (data[i*maxsize+j]-sum) / data[i*maxsize+i];
            }
        }
        for ( int i = 0; i < actualsize; i++ )  // invert L
            for ( int j = i; j < actualsize; j++ )  {
                D x = 1.0;
                if ( i != j ) {
                    x = 0.0;
                    for ( int k = i; k < j; k++ ) 
                        x -= data[j*maxsize+k]*data[k*maxsize+i];
                }
                data[j*maxsize+i] = x / data[j*maxsize+j];
            }
            for ( int i = 0; i < actualsize; i++ )   // invert U
                for ( int j = i; j < actualsize; j++ )  {
                    if ( i == j ) continue;
                    D sum = 0.0;
                    for ( int k = i; k < j; k++ )
                        sum += data[k*maxsize+j]*( (i==k) ? 1.0 : data[i*maxsize+k] );
                    data[i*maxsize+j] = -sum;
                }
                for ( int i = 0; i < actualsize; i++ )   // final inversion
                    for ( int j = 0; j < actualsize; j++ )  {
                        D sum = 0.0;
                        for ( int k = ((i>j)?i:j); k < actualsize; k++ )  
                            sum += ((j==k)?1.0:data[j*maxsize+k])*data[k*maxsize+i];
                        data[j*maxsize+i] = sum;
                    }
    };
};
#endif // MATRIX_H

