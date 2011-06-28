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
#include <vidlib/VideoStreamAdapter.h>

namespace vidlib 
{
    class OsgStream;

    UTILS_PUSH_WARNINGS
    UTILS_DISABLE_DLL_INTERFACE_WARNING
    //! Adapter strumienia z vidliba dla potrzeb OSG. Potrafi optymalizowaæ dostêp do obrazków.
    class VIDLIB_EXPORT __OsgAdapter
    {
        friend class OsgStream;
    private:
        //! Informacje dotycz¹ce u¿ycia strumienia.
        struct Entry
        {
            osg::observer_ptr<VideoImage> image;
            osg::observer_ptr<osg::Texture2D> texture2D;
            osg::observer_ptr<osg::TextureRectangle> textureRect;
        };
        //! Obrazki powsta³e na bazie strumienia.
        typedef std::map< PixelFormat, Entry> Images;
        //! Obrazki powsta³e na bazie strumienia.
        mutable Images images;
        //!
        mutable VideoStream* stream;

    protected:
        //! Konstruktor zeruj¹cy.
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
        //! Odœwie¿a obrazki korzystaj¹ce z danego strumienia.
        void refreshImages();

        virtual osg::ref_ptr<VideoImage> createImage(PixelFormat format) const;

    private:
        //!
        Entry& getEntry(PixelFormat format) const;
        //!
        osg::ref_ptr<VideoImage> getImage( Entry &entry, PixelFormat format ) const;
        //!
        VideoStream* asStream() const;
    };
    UTILS_POP_WARNINGS

    UTILS_PUSH_WARNINGS
    UTILS_DISABLE_DLL_INTERFACE_WARNING
    //! Metody z premedytacj¹ nie s¹ wirtualne tak, aby odwo³ania do nich (znaj¹c typ OsgStream)
    //! mog³y byæ wydajniejsze. Oczywiœcie NIE powinno siê dziedziczyæ po tym typie.
    class VIDLIB_EXPORT OsgStream : public VideoStreamAdapter, public __OsgAdapter, public osg::Referenced
    {
    private:
        double currentTimestamp;

    public:
        //! \param innerStream Strumieñ przejmowany na w³asnoœæ.
        OsgStream(VideoStream* innerStream);
        //!
        virtual ~OsgStream();

    protected:
        //!
        OsgStream(const OsgStream& stream);

    // VideoStream
    public:
        //!
        virtual VideoStream* clone() const;
        //! \param time Pozycja w Ÿródle
        bool setTime(double time);

    protected:
        virtual osg::ref_ptr<VideoImage> createImage(PixelFormat format) const;
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

        //! Aktualizuje obrazki korzystaj¹ce ze strumienia.
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
    UTILS_POP_WARNINGS

} // namespace vidlib


#endif  // HEADER_GUARD_VIDLIB__OSGSTREAMADAPTER_H__