#ifndef DATA_TRANSFER_H
#define DATA_TRANSFER_H

#include <osg/Vec3>
#include <stdio.h>
#include <string>

class CDataTransfer 
{
public:
    enum eTransferType{ FILE_SAVE, FILE_LOAD, MEM_SAVE, MEM_LOAD, GET_SIZE };

    CDataTransfer( eTransferType Type ) {  // Used only with GET_SIZE
        SetMode( Type ); 
    }
    CDataTransfer( void *pData, eTransferType Type, int nMax) { // MEM_SAVE or MEM_LOAD
        m_pData = (unsigned char *)pData;
        m_nMaxSize = nMax;
        SetMode( Type ); 
    }
    CDataTransfer( struct _iobuf *pFile, eTransferType Type ) { // FILE_SAVE or FILE_LOAD
        m_pFile = pFile;
        SetMode( Type ); 
    }
    bool IsRead( ) {
        if (m_Type == FILE_LOAD || m_Type == MEM_LOAD) return true;
        return false;
    }
    void SetMode( eTransferType Type )
    {
        m_Type = Type;
        m_nCount = 0;
        m_bReadOnly = false;
    }
    int NumBytes( )			 {	return m_nCount;    }
    // If read only is set, read pointers are advanced but no data is written
    void ReadOnly( int bOn ) {	m_bReadOnly = bOn;	}

    // This function does all the data transfer
    void CDataTransfer::Data( void *pData, int nBytes )
    {
        switch ( m_Type )
        {
            case FILE_SAVE:
                fwrite (pData, nBytes, 1, m_pFile ); 
                break;
            case FILE_LOAD:
                if ( !m_bReadOnly ) fread( pData, nBytes, 1, m_pFile ); 
                else fseek ( m_pFile, nBytes, SEEK_CUR);
                break;
            case MEM_SAVE:
                if (m_nCount + nBytes > m_nMaxSize) return; 
                memcpy (m_pData+m_nCount, pData, nBytes ); 
                break;
            case MEM_LOAD:
                if (m_nCount + nBytes > m_nMaxSize) return; 
                if ( !m_bReadOnly ) memcpy (pData, m_pData+m_nCount, nBytes);
                break;
        }
        m_nCount += nBytes;
    }

    // Overload all commonly used variables
    void Var( int &n )			{Data( &n, sizeof(int) ); }
    void Var( unsigned int &n )	{Data( &n, sizeof(unsigned int) ); }
    void Var( float &f )		{Data( &f, sizeof(float) ); }	
    void Var( short &n )		{Data( &n, sizeof(short) ); }
    void Var( unsigned char &n ){Data( &n, sizeof(unsigned char) ); }
    void Var( osg::Vec3 &V )		{Data( &V, sizeof(osg::Vec3) ); }

    CDataTransfer& operator<< ( int &n ) { Data( &n, sizeof(int) ); return *this;}
    CDataTransfer& operator<< ( float &n ) { Data( &n, sizeof(float) ); return *this;}
    CDataTransfer& operator<< ( unsigned int &n ) { Data( &n, sizeof(unsigned int) ); return *this;}
    CDataTransfer& operator<< ( short &n ) {Data( &n, sizeof(short) ); return *this;}
    CDataTransfer& operator<< ( char &n ) {Data( &n, sizeof(char) ); return *this;}
    CDataTransfer& operator<< ( unsigned char &n ) {Data( &n, sizeof(unsigned char) ); return *this;}
    CDataTransfer& operator<< ( osg::Vec3 &V ) { Data( &V, sizeof(osg::Vec3) ); return *this;}

private:
    eTransferType m_Type;
    int m_nCount;
    int m_nMaxSize;
    int m_bReadOnly;
    struct _iobuf *m_pFile;
    unsigned char *m_pData;
};

#endif //DATA_TRANSFER_H