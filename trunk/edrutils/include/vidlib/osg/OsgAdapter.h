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

#include <vidlib/Export.h>
#include <vidlib/PixelFormat.h>
#include <vidlib/osg/VideoImage.h>

namespace vidlib 
{
    UTILS_PUSH_WARNINGS
    UTILS_DISABLE_DLL_INTERFACE_WARNING
    //! Adapter strumienia z vidliba dla potrzeb OSG. Potrafi optymalizowaæ dostêp do obrazków.
    class VIDLIB_EXPORT __OsgAdapter : public osg::Referenced
    {
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

    private:
        //!
        Entry& getEntry(PixelFormat format) const;
        //!
        osg::ref_ptr<VideoImage> getImage( Entry &entry, PixelFormat format ) const;
        //!
        VideoStream* asStream() const;
    };
    UTILS_POP_WARNINGS

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

} // namespace vidlib


#endif  // HEADER_GUARD_VIDLIB__OSGSTREAMADAPTER_H__