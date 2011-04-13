/********************************************************************
    created:  2011/04/12
    created:  12:4:2011   21:20
    filename: VideoStreamAdapter.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_VIDLIB__VIDEOSTREAMADAPTER_H__
#define HEADER_GUARD_VIDLIB__VIDEOSTREAMADAPTER_H__

#include <vidlib/VideoStream.h>

namespace vidlib 
{
    //! Adapter strumienia, umo¿liwiaj¹cy zunifikowany dostêp do strumieni
    //! kontroluj¹cych inne strumienie (bufory, osg)
    class VIDLIB_EXPORT VideoStreamAdapter : public VideoStream
    {
    private:
        //! Wewnêtrzny strumieñ.
        VideoStream* innerStream;

    public:
        //! \param innerStream Strumieñ przejmowany na w³asnoœæ.
        VideoStreamAdapter(VideoStream* innerStream);
        //!
        virtual ~VideoStreamAdapter();

    protected:
        //! Prywatny konstruktor do klonowania. Wykonuje g³êbok¹ kopiê.
        VideoStreamAdapter(const VideoStreamAdapter& adapter);

    public:
        //! \return Wewnêtrzny strumieñ.
        inline const VideoStream* getInnerStream() const
        {
            return innerStream;
        }

    // VideoStream
    public:
        //!
        virtual VideoStream* clone() const;
        //! \param time Pozycja w Ÿródle
        virtual bool setTime(double time);
        //! Odczytuje nastêpn¹ klatkê.
        virtual bool readNext();
        //! \retrun Pozycja w strumieniu.
        virtual double getTime() const;
        //! \return Prawdziwy timestamp ramki.
        virtual double getFrameTimestamp() const;
        //! \return Prawdziwy timestamp kolejnej ramki.
        virtual double getNextFrameTimestamp() const;
        //!
        virtual bool getData(Picture& dst) const;
        //!
        virtual bool getData(PictureLayered& dst) const;
    };
} // namespace vidlib


#endif  // HEADER_GUARD_VIDLIB__VIDEOSTREAMADAPTER_H__