/********************************************************************
	created:  2011/01/27
	created:  27:1:2011   10:35
	filename: FFmpegImageStream.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_VIDLIB__FFMPEGIMAGESTREAM_H__
#define __HEADER_GUARD_VIDLIB__FFMPEGIMAGESTREAM_H__

#include <vidlib/osg/GenericImageStream.h>
#include <vidlib/FFmpegVideoStream.h>

////////////////////////////////////////////////////////////////////////////////
namespace vidlib {
////////////////////////////////////////////////////////////////////////////////

class VIDLIB_EXPORT FFmpegImageStream : public GenericImageStream<FFmpegVideoStream>
{
public:
    /** Domyœlne implementacje */
    META_Object(vidlib, FFmpegImageStream);

    /** Konstruktor zeruj¹cy */
    FFmpegImageStream()
    {}
    /** Konstruktor kopiuj¹cy */
    FFmpegImageStream(const FFmpegImageStream& image, const osg::CopyOp & copyop = osg::CopyOp::SHALLOW_COPY) :
    GenericImageStream(image, copyop)
    {}

public:
    //! Lock manager który wykorzystuje muteksy z OpenThreads.
    //! \see FFmpegVideoStream::setLockManager
    static int lockManager(void **mutex, FFmpegVideoStream::LockOp op);
    //! Callback który przekierowywuje logi z ffmpeg do strumieni OSG. 
    //! \see FFmpegVideoStream::setLogCallback
    static void logCallback(FFmpegVideoStream::LogSeverity severity, const char* msg, FFmpegVideoStream::FFmpegClass* item, FFmpegVideoStream::FFmpegClass* parent);
};

////////////////////////////////////////////////////////////////////////////////
} // namespace vidlib
////////////////////////////////////////////////////////////////////////////////

#endif  // __HEADER_GUARD_VIDLIB__FFMPEGIMAGESTREAM_H__