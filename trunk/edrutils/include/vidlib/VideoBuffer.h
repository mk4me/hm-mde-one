/********************************************************************
	created:  2010/07/28
	created:  28:7:2010   14:40
	filename: VideoBufferImpl.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_VIDLIB__VIDEOBUFFER_H__
#define __HEADER_GUARD_VIDLIB__VIDEOBUFFER_H__

#include <list>
#include <vector>
#include <vidlib/Export.h>


////////////////////////////////////////////////////////////////////////////////
namespace vidlib {
////////////////////////////////////////////////////////////////////////////////

class VideoBufferChunk;

//! Bufor ramek o maksymalnym zadanym rozmiarze.
class VIDLIB_EXPORT VideoBuffer
{
private:
    //! Zastosowany wzorzec "Pimpl" (�eby usun�� warningi przy budowie DLL)
    class VideoBufferImpl* impl;

public:
    //! \param maxSize
    //! \param width
    //! \param height
    //! \param format
    VideoBuffer(int maxSize, int width, int height, PixelFormat format);
    //! Destruktor, zwalnia ca�y bufor.
    ~VideoBuffer();

    //! \return
    PixelFormat getFormat() const;

    //! \return Wska�nik na bufor ramki albo NULL, je�eli wyczerpa�o si� miejsce w buforze.
    Picture * pop();
    //! \param frame Ramka, kt�ra ma by� zwr�cona do bufora. Wywo�ywa� TYLKO, je�li po u�yciu
    //!              metody pop ramka nie zosta�a u�yta.
    void push(Picture * frame);

    //! Zaznacza, �e ramka jest u�yta.
    //! \param frame
    //! \param startTime Pocz�tek przedzia�u czasu.
    //! \param endTime Koniec przedzia�u czasu.
    bool notifyUsed(Picture * frame, double startTime, double endTime);

    //! Zwalania ramk� w zadanym punkcie czasowym.
    bool freeFrame(double time);
    //!
    bool freeFirstFrame();
    //!
    bool freeLastFrame();

    //! \param timestamp
    //! \param finish
    const Picture * getLastFrame(double * timestamp = NULL, double * finish = NULL);
    //! \param timestamp
    //! \param finish
    const Picture * getFirstFrame(double * timestamp = NULL, double * finish = NULL);

    //! \param time
    //! \param timestamp
    //! \param finish
    //! \return Zbuforowana ramka dla zdanego czasu b�d� NULL, je�eli nie jest zbuforowana.
    const Picture * getFrame(double time, double * timestamp = NULL, double * finish = NULL);
    //! \param time
    //! \param timestamp
    //! \param finish
    //! \return Zbuforowana ramka dla zdanego czasu b�d� NULL, je�eli nie jest zbuforowana.
    const Picture * getNearestFrame(double time, double * timestamp = NULL, double * finish = NULL);
};

////////////////////////////////////////////////////////////////////////////////
} // namespace vidlib
////////////////////////////////////////////////////////////////////////////////


#endif  // __HEADER_GUARD_VIDLIB__VIDEOBUFFER_H__