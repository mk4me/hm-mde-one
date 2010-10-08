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
    //! Strumieñ który jest buforowany.
    VideoStream * innerStream;
    //! Maksymalny rozmiar bufora.
    int maxBufferSize;
    //! Wybrany czas (pomiêdzy bie¿¹cym faktycznym timestampem a czasem nastêpnej ramki).
    double wantedTime;

public:
    //! 
    //! \param innerStream Strumieñ buforowany. Przejmowany na w³aœnoœæ.
    //! \param maxBufferSize Maksymalny rozmiar bufora.
    BufferedVideoStream(VideoStream * innerStream, int maxBufferSize = INT_MAX);
    //! Zwalnia bufor oraz strumieñ wewnêtrzny.
    virtual ~BufferedVideoStream();

    //! Odczytuje nastêpn¹ ramkê.
    virtual bool readNext();
    //! \retrun Pozycja w strumieniu.
    virtual double getTime() const;
    //! \param time Pozycja w Ÿródle
    virtual bool setTime(double time);
    //! \return Prawdziwy timestamp ramki.
    virtual double getFrameTimestamp() const;
    //! \return Prawdziwy timestamp ramki.
    virtual double getNextFrameTimestamp() const;

public:
    //! Callback wewnêtrznego strumienia.
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