#include "VideoPCH.h"
#include <core/Plugin.h>
#include "VideoParser.h"
#include <vidlib/osg/FFmpegImageStream.h>
#include "VideoVisualizer.h"

CORE_PLUGIN_BEGIN("video", UID::GenerateUniqueID("{4F39348A-3364-4B2F-B748-B1CF578C0628}"));
    CORE_PLUGIN_ADD_PARSER(VideoParser);
    CORE_PLUGIN_ADD_VISUALIZER(VideoVisualizer);
    CORE_PLUGIN_ADD_OBJECT_WRAPPER(::VideoStream);
    CORE_PLUGIN_ADD_OBJECT_WRAPPER(vidlib::Picture);
	CORE_PLUGIN_ADD_OBJECT_WRAPPER(VideoChannel);
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
            UTILS_ASSERT(false, "Nieznany poziom FFmpeg. Wiadomość: %s", msg);
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
