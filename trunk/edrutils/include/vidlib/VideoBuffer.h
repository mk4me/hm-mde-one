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
    //! Zastosowany wzorzec "Pimpl" (¿eby usun¹æ warningi przy budowie DLL)
    class VideoBufferImpl* impl;

public:
    //! \param maxSize
    //! \param width
    //! \param height
    //! \param format
    VideoBuffer(int maxSize, int width, int height, PixelFormat format);
    //! Destruktor, zwalnia ca³y bufor.
    ~VideoBuffer();

    //! \return
    PixelFormat getFormat() const;

    //! \return WskaŸnik na bufor ramki albo NULL, je¿eli wyczerpa³o siê miejsce w buforze.
    Picture * pop();
    //! \param frame Ramka, która ma byæ zwrócona do bufora. Wywo³ywaæ TYLKO, jeœli po u¿yciu
    //!              metody pop ramka nie zosta³a u¿yta.
    void push(Picture * frame);

    //! Zaznacza, ¿e ramka jest u¿yta.
    //! \param frame
    //! \param startTime Pocz¹tek przedzia³u czasu.
    //! \param endTime Koniec przedzia³u czasu.
    bool notifyUsed(Picture * frame, double startTime, double endTime);

    //! Zwalania ramkê w zadanym punkcie czasowym.
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
    //! \return Zbuforowana ramka dla zdanego czasu b¹dŸ NULL, je¿eli nie jest zbuforowana.
    const Picture * getFrame(double time, double * timestamp = NULL, double * finish = NULL);
    //! \param time
    //! \param timestamp
    //! \param finish
    //! \return Zbuforowana ramka dla zdanego czasu b¹dŸ NULL, je¿eli nie jest zbuforowana.
    const Picture * getNearestFrame(double time, double * timestamp = NULL, double * finish = NULL);
};

////////////////////////////////////////////////////////////////////////////////
} // namespace vidlib
////////////////////////////////////////////////////////////////////////////////


#endif  // __HEADER_GUARD_VIDLIB__VIDEOBUFFER_H__