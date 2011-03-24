#include "VideoPCH.h"
#include <core/Plugin.h>
#include <plugins/video/VideoService.h>
#include "VideoParser.h"
#include <core/Log.h>
#include <vidlib/osg/FFmpegImageStream.h>

CORE_PLUGIN_BEGIN("video", UniqueID('VIDE', 'PLUG'));
CORE_PLUGIN_ADD_SERVICE(VideoService);
CORE_PLUGIN_ADD_PARSER(VideoParser);
CORE_PLUGIN_END;

//! \param severity
//! \param msg
//! \param item
//! \param parent
void logCallback(vidlib::FFmpegVideoStream::LogSeverity severity, const char* msg, vidlib::FFmpegVideoStream::FFmpegClass* item, vidlib::FFmpegVideoStream::FFmpegClass* parent)
{
    using namespace vidlib;
    switch ( severity ) {
        case FFmpegVideoStream::LogSeverityQuiet:
            break;

        case FFmpegVideoStream::LogSeverityPanic:
        case FFmpegVideoStream::LogSeverityFatal:
        case FFmpegVideoStream::LogSeverityError:
            LOG_ERROR_STATIC_NAMED("ffmpeg", msg);
            break;

        case FFmpegVideoStream::LogSeverityWarning:
            LOG_WARNING_STATIC_NAMED("ffmpeg", msg);
            break;

        case FFmpegVideoStream::LogSeverityInfo:
        case FFmpegVideoStream::LogSeverityVerbose:
            LOG_INFO_STATIC_NAMED("ffmpeg", msg);
            break;

        case FFmpegVideoStream::LogSeverityDebug:
            LOG_DEBUG_STATIC_NAMED("ffmpeg", msg);
            break;

        default:
            UTILS_ASSERT(false, "Nieznany poziom FFmpeg. Wiadomoœæ: %s", msg);
            break;
    }
}

static struct Initializer
{
    Initializer()
    {
        vidlib::FFmpegVideoStream::setLogCallback(/*vidlib::FFmpegImageStream::logCallback*/logCallback);
        vidlib::FFmpegVideoStream::setLockManager(vidlib::FFmpegImageStream::lockManager);
    }
} initializer;