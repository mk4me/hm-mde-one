/********************************************************************
    created:  2011/03/29
    created:  29:3:2011   14:12
    filename: OsgAdapter.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_VIDLIB__OSGSTREAMADAPTER_H__
#define HEADER_GUARD_VIDLIB__OSGSTREAMADAPTER_H__

#include <map>
#include <string>
#include <utils/Utils.h>

UTILS_PUSH_WARNINGS
#include <osg/ref_ptr>
#include <osg/Referenced>
#include <osg/Texture2D>
#include <osg/TextureRectangle>
UTILS_POP_WARNINGS

#include <vidlib/VideoStream.h>
#include <vidlib/Export.h>
#include <vidlib/PixelFormat.h>
#include <vidlib/osg/VideoImage.h>

namespace vidlib 
{
    UTILS_PUSH_WARNINGS
    UTILS_DISABLE_DLL_INTERFACE_WARNING
    //! Adapter strumienia z vidliba dla potrzeb OSG. Potrafi optymalizowa� dost�p do obrazk�w.
    class VIDLIB_EXPORT __OsgAdapter : public osg::Referenced
    {
    private:
        //! Informacje dotycz�ce u�ycia strumienia.
        struct Entry
        {
            osg::observer_ptr<VideoImage> image;
            osg::observer_ptr<osg::Texture2D> texture2D;
            osg::observer_ptr<osg::TextureRectangle> textureRect;
        };
        //! Obrazki powsta�e na bazie strumienia.
        typedef std::map< PixelFormat, Entry> Images;
        //! Obrazki powsta�e na bazie strumienia.
        mutable Images images;
        //!
        mutable VideoStream* stream;

    protected:
        //! Konstruktor zeruj�cy.
        __OsgAdapter();
        //! Pusty polimorficzny destruktor.
        virtual ~__OsgAdapter();

    public:
        //! \return Obrazek dla zadanego formatu.
        osg::ref_ptr<VideoImage> getImage(PixelFormat format) const;

        //!
        osg::ref_ptr<osg::Texture> getTexture(PixelFormat format, bool textureRect) const;
        //! \return Obrazek dla zadanego formatu.
        osg::ref_ptr<osg::Texture2D> getTexture2D(PixelFormat format) const;
        //! \return Obrazek dla zadanego formatu.
        osg::ref_ptr<osg::TextureRectangle> getTextureRect(PixelFormat format) const;


    protected:
        //! Od�wie�a obrazki korzystaj�ce z danego strumienia.
        void refreshImages();

    private:
        //!
        Entry& getEntry(PixelFormat format) const;
        //!
        osg::ref_ptr<VideoImage> getImage( Entry &entry, PixelFormat format ) const;
        //!
        VideoStream* asStream() const;
    };
    UTILS_POP_WARNINGS

    //! Metody z premedytacj� nie s� wirtualne tak, aby odwo�ania do nich (znaj�c typ OsgStream)
    //! mog�y by� wydajniejsze. Oczywi�cie NIE powinno si� dziedziczy� po tym typie.
    class VIDLIB_EXPORT OsgStream : public VideoStream, public __OsgAdapter
    {
    private:
        //! Wewn�trzny strumie�.
        VideoStream* innerStream;

    public:
        //! \param innerStream Strumie� przejmowany na w�asno��.
        OsgStream(VideoStream* innerStream);
        //!
        virtual ~OsgStream();

    public:
        //! \return Wewn�trzny strumie�.
        inline const VideoStream* getInnerStream() const
        {
            return innerStream;
        }

    // VideoStream
    public:

        //!
        virtual VideoStream* clone() const;

        //! \param time Pozycja w �r�dle
        bool setTime(double time);
        //! Odczytuje nast�pn� klatk�.
        inline bool readNext()
        {
            return innerStream->readNext();
        }
        //! \retrun Pozycja w strumieniu.
        inline double getTime() const
        {
            return innerStream->getTime();
        }
        //! \return Prawdziwy timestamp ramki.
        inline double getFrameTimestamp() const
        {
            return innerStream->getFrameTimestamp();
        }
        //! \return Prawdziwy timestamp kolejnej ramki.
        inline double getNextFrameTimestamp() const
        {
            return innerStream->getNextFrameTimestamp();
        }

    // VideoStream
    protected:
        //!
        bool getData(Picture& dst);
        //!
        bool getData(PictureLayered& dst);
    };

    //! Adapter strumienia z vidliba dla potrzeb OSG.
    template <class Stream>
    class OsgAdapter : public Stream, public __OsgAdapter
    {
    public:
        //! Konstruktor.
        OsgAdapter(const std::string& source) :
        Stream(source)
        {}
        //! Pusty polimorficzny destruktor.
        virtual ~OsgAdapter()
        {}

        //! Aktualizuje obrazki korzystaj�ce ze strumienia.
        virtual bool setTime(double time)
        {
            double prevTimestamp = getFrameTimestamp();
            bool result = Stream::setTime(time);
            if ( prevTimestamp != getFrameTimestamp() ) {
                refreshImages();
            }
            return result;
        }
    };

} // namespace vidlib


#endif  // HEADER_GUARD_VIDLIB__OSGSTREAMADAPTER_H__