/********************************************************************
	created:  2010/07/20
	created:  20:7:2010   16:31
	filename: BufferedVideoStream.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__BUFFEREDVIDEOSTREAM_H__
#define __HEADER_GUARD__BUFFEREDVIDEOSTREAM_H__

#include <plugins/video/core/VideoStream.h>

////////////////////////////////////////////////////////////////////////////////
namespace vm {
////////////////////////////////////////////////////////////////////////////////

class VideoBuffer;

class BufferedVideoStream : public VideoStream, public VideoStream::Callback
{
private:
    //! Bufor na ramki
    VideoBuffer * convertedFrames;
    //! Bufor na ramki jeszcze nie przekonwertowane.
    VideoBuffer * pendingFrames;
    //! Strumie� kt�ry jest buforowany.
    VideoStream * innerStream;
    //! Maksymalny rozmiar bufora.
    int maxBufferSize;
    //! Wybrany czas (pomi�dzy bie��cym faktycznym timestampem a czasem nast�pnej ramki).
    double wantedTime;

public:
    //! 
    //! \param innerStream Strumie� buforowany. Przejmowany na w�a�no��.
    //! \param maxBufferSize Maksymalny rozmiar bufora.
    BufferedVideoStream(VideoStream * innerStream, int maxBufferSize = INT_MAX);
    //! Zwalnia bufor oraz strumie� wewn�trzny.
    virtual ~BufferedVideoStream();

    //! Odczytuje nast�pn� ramk�.
    virtual bool readNext();
    //! \retrun Pozycja w strumieniu.
    virtual double getTime() const;
    //! \param time Pozycja w �r�dle
    virtual bool setTime(double time);
    //! \return Prawdziwy timestamp ramki.
    virtual double getFrameTimestamp() const;
    //! \return Prawdziwy timestamp ramki.
    virtual double getNextFrameTimestamp() const;

public:
    //! Callback wewn�trznego strumienia.
    void frameRead(Picture * picture, PictureLayered * layers, double timestamp, double endTimestamp);

private:
    //!
    Picture* popBuffer( double time );
    //! 
    //! \param result
    bool returnOrCopyError(bool result);
    //!
    virtual bool getData(Picture & dst);

    //const BufferedFrame * getFrame(double time) const;
};

////////////////////////////////////////////////////////////////////////////////
} // namespace vm
////////////////////////////////////////////////////////////////////////////////

#endif  // __HEADER_GUARD__BUFFEREDVIDEOSTREAM_H__