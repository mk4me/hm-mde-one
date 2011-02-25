#include "PCH.h"
#include <math.h>
#include <utils/Debug.h>
#include <utils/Profiler.h>
#include <utils/Enum.h>
#include <vidlib/VideoStream.h>
#include <vidlib/Converter.h>
#include "VidLibPrivate.h"
#include <vidlib/FFmpegVideoStream.h>
#include <vidlib/BufferedVideoStream.h>


////////////////////////////////////////////////////////////////////////////////
namespace vidlib {
////////////////////////////////////////////////////////////////////////////////

VideoStream::VideoStream( const std::string& source )
{
    VIDLIB_FUNCTION_PROLOG;
    this->source = source;
    lastError = NULL;
    frameRate = 0.0;
    frameDuration = 0.0;
    duration = 0.0;
    format = PixelFormatYV12;
    width = 0;
    height = 0;
    aspectRatio = 0.0;
    converter = new Converter();
    callback = NULL;
}


VideoStream::~VideoStream()
{
    VIDLIB_FUNCTION_PROLOG;
    utils::deletePtr(lastError);
    utils::deletePtr(converter);
}


bool VideoStream::notifyError( const VideoError& error )
{
    VIDLIB_FUNCTION_PROLOG;
#ifdef VIDLIB_ENABLE_EXCEPTIONS
    throw error;
#else   // VIDLIB_ENABLE_EXCEPTIONS
    utils::deletePtr(lastError);
    lastError = error.clone();
#endif  // VIDLIB_ENABLE_EXCEPTIONS
    return false;
}


bool VideoStream::onAfterInit( const std::string& /*source*/, double frameRate, double duration, PixelFormat format, int width, int height, double aspectRatio )
{
    VIDLIB_FUNCTION_PROLOG;
    this->frameRate = frameRate;
    this->frameDuration = 1.0 / frameRate;
    this->duration = duration;
    this->format = format;
    this->width = width;
    this->height = height;
    this->aspectRatio = aspectRatio;
    return true;
}

const VideoError * VideoStream::getLastError() const
{
    VIDLIB_FUNCTION_PROLOG;
#ifdef VIDLIB_ENABLE_EXCEPTIONS
    UTILS_FAIL("Should not be here...");
    return NULL;
#else   // VIDLIB_ENABLE_EXCEPTIONS
    return lastError;
#endif  // VIDLIB_ENABLE_EXCEPTIONS
}


// bool VideoStream::getFrame( FrameData & dst )
// {
//   // najpierw pr�bujemy pobra� bie��c� ramk�
//   FrameData src;
//   if ( getCurrentFrameData(src) ) {
//     return converter->convert( src, dst );
//   } else {
//     // je�eli strumie� nie jest w stanie upakowa� danych do ramki pr�bujemy
//     // skonwertowa� obie ramki do postaci zdj�cia
//     FrameLayered srcPicture, dstPicture;
//     if ( getCurrentFrameData(srcPicture) && dst.translate(&dstPicture) ) {
//       return converter->convert(srcPicture, dstPicture);
//     } else {
//       VIDLIB_ERROR( VideoError("Conversion not possible.") );
//     }
//   }
// }

bool VideoStream::getFrame( Picture & dst )
{
    VIDLIB_FUNCTION_PROLOG;
    if ( getTime() < 0.0 || getTime() > getDuration() ) {
        VIDLIB_ERROR( VideoError("Stream time is out of bounds.") );
    }
    Picture src;
    if ( getData(src) ) {
        return converter->convert( src, dst );
    } else {
        // je�eli strumie� nie jest w stanie upakowa� danych do ramki pr�bujemy
        // skonwertowa� obie ramki do postaci zdj�cia
        PictureLayered layers = dst.getLayers();
        return getFrame(layers);
    }
}

bool VideoStream::getFrame( PictureLayered & dst )
{
    VIDLIB_FUNCTION_PROLOG;
    if ( getTime() < 0.0 || getTime() > getDuration() ) {
        VIDLIB_ERROR( VideoError("Stream time is out of bounds.") );
    }
    // je�eli strumie� nie jest w stanie upakowa� danych do ramki pr�bujemy
    // skonwertowa� obie ramki do postaci zdj�cia
    PictureLayered srcPicture;
    if ( getData(srcPicture) ) {
        return converter->convert(srcPicture, dst);
    } else {
        VIDLIB_ERROR( VideoError("Conversion not possible.") );
    }
}

void VideoStream::onFrameRead()
{
    VIDLIB_FUNCTION_PROLOG;
    if ( callback ) {
        Picture picture;
        Picture * passedPicture = getData(picture) ? &picture : NULL;
        PictureLayered layers;
        PictureLayered * passedLayers = getData(layers) ? &layers : NULL;
        callback->frameRead(passedPicture, passedLayers, getFrameTimestamp(), getNextFrameTimestamp());
    }
}

int VideoStream::getFrameCount() const
{
    VIDLIB_FUNCTION_PROLOG;
    return static_cast<int>(getDuration() / getFrameDuration() + 0.5);
}

double VideoStream::getNormalizedTime() const
{
    double result = getTime() / getDuration();
    UTILS_ASSERT(result >= 0.0 && result <= 1.0);
    return result;
}

void VideoStream::setNormalizedTime( double normalizedTime )
{
    UTILS_ASSERT(normalizedTime >= 0.0 && normalizedTime <= 1.0);
    setTime( normalizedTime * getDuration() );
}

////////////////////////////////////////////////////////////////////////////////
} // namespace vidlib
////////////////////////////////////////////////////////////////////////////////