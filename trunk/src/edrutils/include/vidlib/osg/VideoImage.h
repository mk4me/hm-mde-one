/********************************************************************
    created:  2011/03/29
    created:  29:3:2011   14:22
    filename: VideoImage.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_VIDLIB__VIDEOIMAGE_H__
#define HEADER_GUARD_VIDLIB__VIDEOIMAGE_H__

#include <vidlib/Config.h>
#include <utils/Utils.h>
#include <map>
UTILS_PUSH_WARNINGS
#include <osg/ref_ptr>
#include <osg/observer_ptr>
#include <osg/Image>
UTILS_POP_WARNINGS
#include <vidlib/Export.h>
#include <vidlib/PixelFormat.h>
#include <vidlib/VideoStream.h>
#include <vidlib/Picture.h>

namespace vidlib 
{
    UTILS_PUSH_WARNINGS
    UTILS_DISABLE_DLL_INTERFACE_WARNING

    class VIDLIB_EXPORT VideoImage : public osg::Image
    {
    private:
        //! Klienci maksymalnego rozmiaru.
        typedef std::map<osg::observer_ptr<osg::Referenced>, int> SizeClients;
        //! Klienci maksymalnego rozmiaru.
        SizeClients sizes;
        //! Minimalna szeroko��.
        int minWidth;
        //! Strumie� video.
        VideoStream* stream;
        //! Obrazek.
        Picture currentPicture;
        //!
        double timestamp;

    public:
        //! \param stream
        //! \param format
        VideoImage(VideoStream* stream, PixelFormat format);
        //!
        virtual ~VideoImage();

    public:
        //! \return Format strumienia.
        inline const PixelFormat getFormat() const
        { 
            return currentPicture.format;
        }
        //! \return Wewn�trzny strumie� video.
        inline VideoStream* getStream()
        { 
            return stream;
        }
        //! \return Wewn�trzny strumie� video.
        inline const VideoStream* getStream() const
        { 
            return stream;
        }
        //! \return Minimalna szeroko��.
        inline int getMinWidth() const
        { 
            return minWidth;
        }
        //! \return
        inline double getTimestamp() const
        { 
            return timestamp;
        }


        //! \param minWidth Minimalna szeroko��. (>= 2)
        void setMinWidth(int minWidth);
        //! Usuwa klienta maksymalnej szeroko�ci.
        bool removeMaxWidth(osg::Referenced* source);
        //! Dodaje klienta maksymalnej szeroko�ci.
        //! \param source �r�d�o.
        //! \param maxWidth Szeroko��.
        void setMaxWidth(osg::Referenced* source, int maxWidth);

        
        //! \return Bie��ca maksymalna szeroko��.
        int getMaxWidth();
        //! Od�wie�a obrazek.
        void refresh();

    private:
        //! Prze�adowuje obrazek.
        void reloadImage(Picture & picture);
        //!
        bool applyMaxWidth( int maxWidth );
    };
    
    //! Uzupe�nienie poprzedniego 
    template <class T, class PtrPolicy>
    class VideoImageSafe : public VideoImage, public PtrPolicy
    {
    public:
        typedef typename PtrPolicy::Ptr<T>::Type Ptr;

    private:
        Ptr referenceHolder;
        
    public:
        VideoImageSafe(const Ptr& ptr, PixelFormat format) : 
        VideoImage( &*ptr, format ), referenceHolder(ptr)
        {}

        virtual ~VideoImageSafe() 
        {}

    };

    UTILS_POP_WARNINGS
} // namespace vidlib

#endif  // HEADER_GUARD_VIDLIB__VIDEOIMAGE_H__