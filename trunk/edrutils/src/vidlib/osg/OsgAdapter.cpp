#include "../PCH.h"
#include "../VidLibPrivate.h"
#include <vidlib/osg/OsgAdapter.h>
#include <boost/foreach.hpp>

namespace vidlib 
{
    
    __OsgAdapter::__OsgAdapter() :
    stream(nullptr)
    {
    }

    __OsgAdapter::~__OsgAdapter()
    {
    }

    void __OsgAdapter::refreshImages()
    {
        BOOST_FOREACH( const Images::value_type& val, images ) {
            osg::ref_ptr<VideoImage> img;
            if (val.second.image.lock(img)) {
                img->refresh();
            }
        }
    }

    osg::ref_ptr<VideoImage> __OsgAdapter::getImage( PixelFormat format ) const
    {
        Entry& entry = getEntry(format);
        return getImage(entry, format);
    }

    osg::ref_ptr<osg::Texture2D> __OsgAdapter::getTexture2D( PixelFormat format ) const
    {
        Entry& entry = getEntry(format);
        osg::ref_ptr<osg::Texture2D> texture;
        if ( entry.texture2D.lock(texture) ) {
            return texture;
        } else {
            texture = new osg::Texture2D(getImage(entry, format));
            texture->setResizeNonPowerOfTwoHint(false);
            entry.texture2D = texture;
            return texture;
        }
    }

    osg::ref_ptr<osg::Texture> __OsgAdapter::getTexture( PixelFormat format, bool textureRect ) const
    {
        if ( textureRect ) {
            return getTextureRect(format);
        } else {
            return getTexture2D(format);
        }
    }

    osg::ref_ptr<osg::TextureRectangle> __OsgAdapter::getTextureRect( PixelFormat format ) const
    {
        Entry& entry = getEntry(format);
        osg::ref_ptr<osg::TextureRectangle> texture;
        if ( entry.textureRect.lock(texture) ) {
            return texture;
        } else {
            texture = new osg::TextureRectangle(getImage(entry, format));
            entry.textureRect = texture;
            return texture;
        }
    }

    __OsgAdapter::Entry& __OsgAdapter::getEntry( PixelFormat format ) const
    {
        return images[format];
    }

    osg::ref_ptr<VideoImage> __OsgAdapter::getImage( Entry &entry, PixelFormat format ) const
    {
        osg::ref_ptr<VideoImage> image;
        if ( entry.image.lock(image) ) {
            return image;
        } else {
            image = new VideoImage(asStream(), format);
            entry.image = image;
            return image;
        }
    }

    inline VideoStream* __OsgAdapter::asStream() const
    {
        if ( !stream ) {
            stream = dynamic_cast<VideoStream*>(const_cast<__OsgAdapter*>(this));
            UTILS_ASSERT(stream);
        }
        return stream;
    }


    OsgStream::OsgStream( VideoStream* innerStream ) : innerStream(innerStream)
    {
        if ( !innerStream ) {
            VIDLIB_ERROR_NORETURN(VideoError("innerStream==null"));
        }
        onAfterInit(innerStream->getSource(), innerStream->getFramerate(), 
            innerStream->getDuration(), innerStream->getPixelFormat(), 
            innerStream->getWidth(), innerStream->getHeight(), 
            innerStream->getAspectRatio());
    }

    OsgStream::~OsgStream()
    {
        utils::deletePtr(innerStream);
    }


    VideoStream* OsgStream::clone() const
    {
        std::auto_ptr<VideoStream> innerClone(innerStream->clone());
        UTILS_ASSERT(innerStream);
        osg::ref_ptr<OsgStream> cloned = new OsgStream(innerClone.release());
        return cloned.release();
    }

    bool OsgStream::setTime( double time )
    {
        double prevTimestamp = getFrameTimestamp();
        bool result = innerStream->setTime(time);
        if ( prevTimestamp != getFrameTimestamp() ) {
            refreshImages();
        }
        return result;
    }

    bool OsgStream::getData( Picture& dst )
    {
        return innerStream->getData(dst);
    }

    bool OsgStream::getData( PictureLayered& dst )
    {
        return innerStream->getData(dst);
    }


} // namespace vidlib