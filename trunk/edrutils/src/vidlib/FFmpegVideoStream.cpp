#include "PCH.h"
#include "VidLibPrivate.h"
#include <vidlib/FFmpegError.h>
#include <vidlib/FFmpegVideoStream.h>

//!
#define AVIO_FFMPE_ENABLE_DUMP_INFO

#ifdef AVIO_FFMPE_ENABLE_DUMP_INFO
#include <iostream>
#endif // AVIO_FFMPE_ENABLE_DUMP_INFO

#include <vector>

#ifdef WIN32
// blok definicji dla ffmpeg'a
#define inline _inline
#define snprintf _snprintf
#define vsnprintf(buffer, count, format, argptr) vsnprintf_s((buffer), (count), _TRUNCATE, (format), (argptr))
#endif

extern "C" {
#pragma warning (push)
#pragma warning (disable: 4244)
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/mathematics.h>
#include <libavutil/log.h>
#pragma warning (pop)
}

//! Ile ramek mo¿na omin¹æ podczas wyszukiwania do przodu?
#define AVIO_FFMPEG_MAXSKIPFRAMES 100
//! Ile ramek mo¿na omin¹æ podczas wyszukiwania do przodu?
#define AVIO_FFMPEG_MINSKIPFRAMES 5
//! Maska u¿ywana podczas alignowania danych
#define AVIO_FFMPEG_ALIGN_MASK (VIDLIB_FFMPEG_ALIGN_BYTES-1)


#define AVIO_FFMPEG_SEEK_WITH_STREAM_INDEX
//#define AVIO_FFMPEG_ENABLE_EXPERIMENTAL_API


#define BREAK_ON_ERROR( x ) { if (!(x)) { VIDLIB_ERROR(*getLastError()); }}


//! Wyzerowany callback.
static vidlib::FFmpegVideoStream::LockManager lockManager = NULL;
static vidlib::FFmpegVideoStream::LogCallback logCallback = NULL;

//! Funkcja przekazuj¹ca ¿¹danie locka do ustawionego callbacka.
//! \param mutex
//! \param op
static int FFmpegLockForwarder( void **mutex, enum AVLockOp op )
{
    using namespace vidlib;
    // t³umaczymy operacjê
    FFmpegVideoStream::LockOp lockOp =
    op == AV_LOCK_CREATE ? FFmpegVideoStream::LockOpCreate :
    op == AV_LOCK_DESTROY ? FFmpegVideoStream::LockOpDestroy :
    op == AV_LOCK_OBTAIN ? FFmpegVideoStream::LockOpObtain : FFmpegVideoStream::LockOpRelease;

    // wywo³anie callbacka
    if ( lockManager ) {
        return lockManager(mutex, lockOp);
    } else {
        return 0;
    }
}

//! Funkcja przekazuj¹ca ¿¹danie logowania do ustawionego callbacka.
//! \param ptr
//! \param level
//! \param fmt
//! \param vl
static void FFmpegLogForwarder(void* ptr, int level, const char* fmt, va_list vl)
{
    using namespace vidlib;

    if (!logCallback) {
        // nie ma callbacka, nic nie robimy
        return;
    }

    FFmpegVideoStream::FFmpegClass itemClass = { NULL, NULL };
    FFmpegVideoStream::FFmpegClass parentClass = { NULL, NULL };

    // wersja wzorowana na av_log_default_callback (log.c)
    static int print_prefix=1;
    static char line[4096] = { 0 };
    static int len = 0;

    AVClass* avc = ptr ? *reinterpret_cast<AVClass**>(ptr) : NULL;

    int avLogLevel = av_log_get_level();
    if(level>avLogLevel) {
        return;
    }

    //line[0]=0;

    // pobieramy wskaŸniki na klasy
    if(avc) {
        if(avc->version >= (50<<16 | 15<<8 | 3) && avc->parent_log_context_offset){
            AVClass** parent= *(AVClass***)(((uint8_t*)ptr) + avc->parent_log_context_offset);
            if(parent && *parent) {
                parentClass.ptr = parent;
                parentClass.name = (*parent)->item_name(parent);
            }
        }
        itemClass.ptr = avc;
        itemClass.name = avc->item_name(ptr);
    }

    // formatowanie wiadomoœci
    int prevLen = len;
    len = vsnprintf(line+len, sizeof(line) - len - 1, fmt, vl);
    if ( len < 0 ) {
        len = sizeof(line);
    } else {
        len += prevLen;
    }

    // czy ostatni znak to nowa linia? 
    print_prefix = line[len-1] == '\n';
    if ( !print_prefix ) {
        // nie?
        return;
    }
    // usuwamy znak nowej linii
    line[len - 1] = 0;

    // przet³umaczenie poziomu wa¿noœci komunikatu
    FFmpegVideoStream::LogSeverity severity;
    if ( level <= AV_LOG_QUIET ) {
        severity = FFmpegVideoStream::LogSeverityQuiet;
    } else if ( level <= AV_LOG_PANIC ) {
        severity = FFmpegVideoStream::LogSeverityPanic;
    } else if ( level < AV_LOG_FATAL ) {
        severity = FFmpegVideoStream::LogSeverityFatal;
    } else if ( level < AV_LOG_ERROR ) {
        severity = FFmpegVideoStream::LogSeverityError;
    } else if ( level < AV_LOG_WARNING ) {
        severity = FFmpegVideoStream::LogSeverityWarning;
    } else if ( level < AV_LOG_INFO ) {
        severity = FFmpegVideoStream::LogSeverityInfo;
    } else if ( level < AV_LOG_VERBOSE ) {
        severity = FFmpegVideoStream::LogSeverityVerbose;
    } else {
        severity = FFmpegVideoStream::LogSeverityDebug;
    }

    // przekierowanie do callbacka w³aœciwego
    logCallback(severity, line, itemClass.ptr ? &itemClass : NULL, parentClass.ptr ? &parentClass : NULL);
    line[0] = 0;
    len = 0;

    //colored_fputs(av_clip(level>>3, 0, 6), line);
    //strcpy(prev, line);

//     void av_log_default_callback(void* ptr, int level, const char* fmt, va_list vl)
//     {
//         static int print_prefix=1;
//         static int count;
//         static char line[1024], prev[1024];
//         AVClass* avc= ptr ? *(AVClass**)ptr : NULL;
//         if(level>av_log_level)
//             return;
//         line[0]=0;
// #undef fprintf
//         if(print_prefix && avc) {
//             if(avc->version >= (50<<16 | 15<<8 | 3) && avc->parent_log_context_offset){
//                 AVClass** parent= *(AVClass***)(((uint8_t*)ptr) + avc->parent_log_context_offset);
//                 if(parent && *parent){
//                     snprintf(line, sizeof(line), "[%s @ %p] ", (*parent)->item_name(parent), parent);
//                 }
//             }
//             snprintf(line + strlen(line), sizeof(line) - strlen(line), "[%s @ %p] ", avc->item_name(ptr), ptr);
//         }
// 
//         vsnprintf(line + strlen(line), sizeof(line) - strlen(line), fmt, vl);
// 
//         print_prefix= line[strlen(line)-1] == '\n';
//         if(print_prefix && !strcmp(line, prev)){
//             count++;
//             fprintf(stderr, "    Last message repeated %d times\r", count);
//             return;
//         }
//         if(count>0){
//             fprintf(stderr, "    Last message repeated %d times\n", count);
//             count=0;
//         }
//         colored_fputs(av_clip(level>>3, 0, 6), line);
//         strcpy(prev, line);
//     }

}

////////////////////////////////////////////////////////////////////////////////
namespace vidlib {
////////////////////////////////////////////////////////////////////////////////

FFmpegVideoStream::Initializer::Initializer()
{
    // rejestracja formatów
    av_register_all();
    // rejestracja callbacka
    av_lockmgr_register( FFmpegLockForwarder );
}

FFmpegVideoStream::Initializer::~Initializer()
{
    // kasujemy callbacka
    av_lockmgr_register( NULL );
}


//------------------------------------------------------------------------------

void FFmpegVideoStream::setLockManager( LockManager manager )
{
    lockManager = manager;
}

//------------------------------------------------------------------------------

void FFmpegVideoStream::setLogCallback( LogCallback callback )
{
    logCallback = callback;
    av_log_set_callback(FFmpegLogForwarder);
}

//------------------------------------------------------------------------------


FFmpegVideoStream::FFmpegVideoStream( const std::string& source, int wantedVideoStream /*= -1*/ )
: VideoStream(source)
{
    VIDLIB_FUNCTION_PROLOG;
    static Initializer initializer;

    formatContext = NULL;
    codecContext = NULL;
    frame = NULL;
    videoStream = NULL;

    frameTimestamp = AV_NOPTS_VALUE;
    frameSpan = AV_NOPTS_VALUE;
    nextFrameTimestamp = AV_NOPTS_VALUE;

    keyframeStats.count = 0;
    keyframeStats.lastTimestamp = AV_NOPTS_VALUE;
    keyframeStats.maxSpan = AV_NOPTS_VALUE;
    keyframeStats.avgSpan = INVALID_TIMESTAMP;


    frameBytesRemaining = 0;
    frameData = NULL;
    packet = NULL;
    alignedPacket = NULL;

    wantedTime = INVALID_TIMESTAMP;
    init( source, wantedVideoStream );

    // sprawdzanie pewnych warunków
    int frameCount = static_cast<int>(videoStream->duration/frameSpan);
    UTILS_ASSERT(frameCount==getFrameCount(), "Frame count does not match (%d vs %d)", frameCount, getFrameCount());
}

//------------------------------------------------------------------------------

FFmpegVideoStream::~FFmpegVideoStream()
{
    VIDLIB_FUNCTION_PROLOG;
    av_free_packet(alignedPacket);
    av_free(alignedPacket);
    av_free(packet);
    av_free(frame);
    avcodec_close(codecContext);
    av_close_input_file(formatContext);
}

//------------------------------------------------------------------------------

bool FFmpegVideoStream::init( const std::string& source, int wantedVideoStream /*= -1*/ )
{
    VIDLIB_FUNCTION_PROLOG;
    int error = 0;
    // mo¿na inicjalizowaæ wiele razy, sprawdza wewnetrznie czy ju¿ by³a zainicjalizowana
    av_register_all();
    // otwieramy plik
    if ( (error = av_open_input_file(&formatContext, source.c_str(), NULL, 0, NULL)) != 0 ) {
        VIDLIB_ERROR(FFmpegError( "av_open_input_file error", error ));
    }
    // info o kodekach
    if ( (error=av_find_stream_info(formatContext)) < 0 ) {
        VIDLIB_ERROR(FFmpegError( "av_find_stream_info error", error ));
    }

#ifdef VIDLIB_DEBUG
    dump_format(formatContext, 0, source.c_str(), false);
#endif // VIDLIB_DEBUG

    // szukamy strumienia video
    for (unsigned int i = 0; i < formatContext->nb_streams; ++i) {
        AVStream * selectedStream = formatContext->streams[i];
        if ( selectedStream->codec->codec_type == CODEC_TYPE_VIDEO ) {
            // czy to jest ten strumieñ, który chcemy otworzyæ?
            if ( wantedVideoStream < 0 || wantedVideoStream == static_cast<int>(i) ) {
                codecContext = selectedStream->codec;
                videoStream = selectedStream;
                break;
            }
        }
    }

    // TODO: ujednolicenie errorów
    if ( codecContext == NULL ) {
        VIDLIB_ERROR( FFmpegError( "Video stream not found." ) );
    }

    // pobieramy kodek
    AVCodec *pCodec = avcodec_find_decoder(codecContext->codec_id);
    if(pCodec==NULL) {
        VIDLIB_ERROR( FFmpegError( "avcodec_find_decoder" ) );
    }

    // otwieramy kodek
    if( (error=avcodec_open(codecContext, pCodec)) <0) {
        VIDLIB_ERROR( FFmpegError( "avcodec_open", error ) );
    }

    // alokacja kaltki
    frame = avcodec_alloc_frame();
    if ( !frame ) {
        VIDLIB_ERROR( FFmpegError( "avcodec_alloc_frame error", error ) );
    }

    // alokacja pakietu
    packet = (AVPacket*)av_malloc(sizeof(AVPacket));
    av_init_packet(packet);
    alignedPacket = (AVPacket*)av_malloc(sizeof(AVPacket));
    utils::zero(*alignedPacket);
    av_init_packet(alignedPacket);

    // odstêp miêdzy ramkami
    AVRational frameSpanRational = av_mul_q(videoStream->time_base, videoStream->r_frame_rate);
    frameSpan = frameSpanRational.den;
    UTILS_ASSERT(frameSpanRational.num == 1, "Error getting frame span.");
    //UTILS_ASSERT(videoStream->duration % frameSpan == 0, "Error getting frame span.");

    // parametry strumienia
    double frameRate = av_q2d(videoStream->r_frame_rate);
    double durationSec = static_cast<double>(videoStream->duration) * videoStream->time_base.num / videoStream->time_base.den;
    PixelFormat format = static_cast<PixelFormat>(codecContext->pix_fmt);

    // format
    double aspectRatio = 0.0f;
    if (codecContext->sample_aspect_ratio.num != 0) {
        aspectRatio = av_q2d(codecContext->sample_aspect_ratio);
    }
    if (aspectRatio <= 0.0f) {
        aspectRatio = 1.0f;
    }

    // wszystko OK
    return onAfterInit(source, frameRate, durationSec, format, codecContext->width, codecContext->height, aspectRatio);
}

//------------------------------------------------------------------------------

bool FFmpegVideoStream::readNext()
{
    VIDLIB_FUNCTION_PROLOG;
    if ( frameTimestamp == AV_NOPTS_VALUE ) {
        if ( !setTime(0.0) ) {
            VIDLIB_ERROR( *getLastError() );
        }
    } else if ( !readFrame() ) {
        VIDLIB_ERROR(*getLastError());
    }
    wantedTime = timestampToSec(frameTimestamp);
    return true;
}

//------------------------------------------------------------------------------

double FFmpegVideoStream::getTime() const
{
    VIDLIB_FUNCTION_PROLOG;
    return wantedTime;
}

//------------------------------------------------------------------------------

bool FFmpegVideoStream::setTime( double time )
{
    VIDLIB_FUNCTION_PROLOG;
    //Measurer measurer("setTime");
    if ( time < 0.0 || time > getDuration() ) {
        // przy debugowaniu mo¿na zakomentowaæ ten b³¹d i wyzerowaæ czas
        VIDLIB_ERROR( FFmpegError("time < 0.0 || time > getDuration()") );
    }
//     if ( time == getDuration() ) {
//         // TODO
//         // ffmpeg Ÿle ustawia siê przy seeku do zerowego czasu (wypluwa ostatni¹ odczytan¹ ramkê)
//         // tymczasowe, do stawiania breakpointów jeœli czas == 0
//         // int brr = 0;
//     }

    // ustawiamy czas
    wantedTime = time;

    // konwersja czasu na jednostki strumienia
    int64_t targetTimestamp = secToTimestap(time);

    // flaga mówi¹ca, czy zrobimy seeka na klatkê kluczow¹ przed zadany czas,
    // a nastêpnie ominiemy klatki
    bool seekThenSkipFrames = true;
    // teraz sprawdzamy
    if ( frameTimestamp == AV_NOPTS_VALUE ) {
        // strumieñ w nieustalonym stanie, musimy po prostu przejœæ
    } else if ( targetTimestamp >= frameTimestamp ) {
        // delta
        int64_t delta = targetTimestamp - frameTimestamp;
        if ( delta < frameSpan ) {
            // timestamp mieœci siê pomiêdzy faktycznym timestampem poprzedniej oraz bie¿¹cej klatki,
            // wiêc nie trzeba nic robiæ
            seekThenSkipFrames = false;
        } else {
            if ( keyframeStats.count > 1 ) {
                // potencjalna liczba ramek do ominiêcia (zaokr¹glone w górê)
                int potentialToSkip = static_cast<int>( (delta+frameSpan-1)/frameSpan );
                if ( potentialToSkip <= AVIO_FFMPEG_MINSKIPFRAMES ) {
                    BREAK_ON_ERROR(skipFramesToTimestamp(targetTimestamp, AVIO_FFMPEG_MINSKIPFRAMES, false));
                } else if (keyframeStats.lastTimestamp != AV_NOPTS_VALUE) {
                    int64_t nextKeyframe = keyframeStats.lastTimestamp + keyframeStats.maxSpan;
                    int64_t timeLeft = nextKeyframe - frameTimestamp;
                    if ( timeLeft >= delta ) {
                        int framesToSkip = static_cast<int>( (timeLeft+frameSpan-1)/frameSpan) + 1;
                        BREAK_ON_ERROR(skipFramesToTimestamp(targetTimestamp, framesToSkip, false));
                    }
                }
            } else {
                // nie mo¿na przewidywaæ; nie pozostaje nic jak leciec do przodu, a w momencie
                // natrafienia na klatkê kluczow¹ siê zatrzymaæ
                BREAK_ON_ERROR(skipFramesToTimestamp(targetTimestamp, INT_MAX, true));
            }
            // sprawdzamy, czy uda³o siê przejœæ
            seekThenSkipFrames = ( nextFrameTimestamp < targetTimestamp );
        }
    }

    if ( seekThenSkipFrames ) {
        // wyszukujemy tamki
        BREAK_ON_ERROR(seekToKeyframe(targetTimestamp, false));
        // dla niektórych strumieni seek zatrzymuje siê za ramk¹, której szukamy;
        // dlatego podejmujemy trzy próby cofniêcia siê jeszcze bardziej
        int64_t target = targetTimestamp;
        for (int i = 4; --i && target > 0 && frameTimestamp > target; ) {
            target = std::max<int64_t>(target-frameSpan, 0);
            UTILS_DEBUG_PRINT("Fixing (%d left) invalid seek (%ld)", i, frameTimestamp-target);
            BREAK_ON_ERROR(seekToKeyframe(target, false));
        }

        if ( nextFrameTimestamp < targetTimestamp ) {
            // przeszliœmy na klatkê przed wybranym czasem, spróbujmy jeszcze raz
            BREAK_ON_ERROR(skipFramesToTimestamp(targetTimestamp, INT_MAX, true));
            if ( nextFrameTimestamp < time ) {
                // zatrzymaliœmy siê na klatce kluczowej, czyli coœ jest nie tak z wyszukiwaniem...
                VIDLIB_ERROR( FFmpegError("Error during seek.") );
            }
        } else {
            // seek nie przeniós³ nas na klatkê kluczow¹ przed zadan¹ ramk¹, wiêc
            // nic nie mo¿emy ju¿ zrobiæ
            return true;
        }
    } else {
        return true;
    }
    return true;
}

//------------------------------------------------------------------------------

double FFmpegVideoStream::getFrameTimestamp() const
{
    VIDLIB_FUNCTION_PROLOG;
    return timestampToSec(frameTimestamp);
}

//------------------------------------------------------------------------------

double FFmpegVideoStream::getNextFrameTimestamp() const
{
    VIDLIB_FUNCTION_PROLOG;
    return timestampToSec(frameTimestamp+frameSpan);
}

//------------------------------------------------------------------------------

bool FFmpegVideoStream::seekToKeyframe( int64_t timestamp, bool pickNextframe )
{
    VIDLIB_FUNCTION_PROLOG;
    {
        //Measurer measurer("seek");
        UTILS_ASSERT(timestamp >= 0.0 && timestamp <= videoStream->duration, "Timestamp out of bounds.");

        // zerowanie zmiennych trzymaj¹cych stan
        frameBytesRemaining = 0;
        frameData = NULL;

        int error = 0;

#ifdef AVIO_FFMPEG_SEEK_WITH_STREAM_INDEX
        int streamIdx = videoStream->index;
        int64_t seekTarget = timestamp;
#else // AVIO_FFMPEG_SEEK_WITH_STREAM_INDEX
        const AVRational timeBase = { 1, AV_TIME_BASE };
        int streamIdx = -1;
        int64_t seekTarget = av_rescale_q( timestamp, videoStream->time_base, timeBase );
#endif // AVIO_FFMPEG_SEEK_WITH_STREAM_INDEX

#ifdef AVIO_FFMPEG_ENABLE_EXPERIMENTAL_API
        // granice
        int64_t seekMin = pickNextframe ? seekTarget : INT64_MIN;
        int64_t seekMax = !pickNextframe ? seekTarget : INT64_MAX;
        // +- 2 wprowadzone z powodu b³êdu zaokr¹gleñ
        // seek
        error = avformat_seek_file(formatContext, streamIdx, seekMin, seekTarget, seekMax, AVSEEK_FLAG_ANY);
#else // AVIO_FFMPEG_ENABLE_EXPERIMENTAL_API
        int flags = (pickNextframe ? 0 : AVSEEK_FLAG_BACKWARD);
        if ( av_seek_frame(formatContext, streamIdx, seekTarget, flags) < 0 ) {
            flags ^= AVSEEK_FLAG_BACKWARD;
            error = av_seek_frame(formatContext, streamIdx, seekTarget, flags);
        }
#endif // AVIO_FFMPEG_ENABLE_EXPERIMENTAL_API

        if (error < 0) {
            VIDLIB_ERROR( FFmpegError("Error during seek.", error) );
        }

        // flush buffers
        avcodec_flush_buffers(codecContext);

        // "symulujemy" timestamp, aby nextFrameTimestamp zosta³ ustawiony
        nextFrameTimestamp = frameTimestamp = AV_NOPTS_VALUE;
        // zerujemy timestamp ramki kluczowej
        keyframeStats.lastTimestamp = AV_NOPTS_VALUE;
    }



    // odczytujemy ramkê
    if ( readFrame() ) {
//     if ( frameTimestamp > timestamp ) {
//       // seek poszedl za daleko, staramy siê wróciæ
//       if (timestamp > 0) {
//         UTILS_DEBUG_PRINT("Fixing invalid seek (%ld)", frameTimestamp-timestamp);
//         return seekToKeyframe( std::max(timestamp - frameSpan, int64_t(0)), false );
//       }
//     }
        return true;
    } else {
        VIDLIB_ERROR( *getLastError() );
    }
}

//------------------------------------------------------------------------------

bool FFmpegVideoStream::readFrame()
{
    VIDLIB_FUNCTION_PROLOG;
    //Measurer measure("readFrame");
    int error = 0;
    int gotPicture = 0;

    for (;;) {

#ifndef VIDLIB_FFMPEG_ENABLE_NEWAPI
        //!
        while ( frameBytesRemaining > 0 ) {
            // odczytujemy ramkê
            const int bytesDecoded = avcodec_decode_video(codecContext, frame, &gotPicture, frameData, frameBytesRemaining);
            if ( bytesDecoded < 0 ) {
                VIDLIB_ERROR( FFmpegError("Error decoding frame") );
            }
            frameBytesRemaining -= bytesDecoded;
            frameData += bytesDecoded;

            if ( gotPicture ) {
                // liczymy timestamp
                frameTimestamp = getTimestamp(frame, packet->dts);
                nextFrameTimestamp = std::min(frameTimestamp+frameSpan, videoStream->duration);
                onGotFrame(frame, frameTimestamp);
                return true;
            }
        }
#endif // VIDLIB_FFMPEG_ENABLE_NEWAPI

        // czy to koniec?
        if ( url_feof(formatContext->pb) ) {
            nextFrameTimestamp = frameTimestamp = videoStream->duration;
            return true;
            //VIDLIB_ERROR( FFmpegError("End of videoStream reached.") );
        }

        // zwalniamy poprzedni pakiet
        av_free_packet(packet);

        // odczytujemy pakiet
        {
            error = av_read_frame(formatContext, packet);
            if ( error < 0 ) {
                if ( error == AVERROR_EOF ) {
                    //VIDLIB_ERROR( FFmpegError("Unexpected EOF", error));
                    nextFrameTimestamp = frameTimestamp = videoStream->duration;
                    return true;
                } else {
                    VIDLIB_ERROR( FFmpegError("Error reading frame", error) );
                }
            }
        }

        // alignujemy dane
        if ( reinterpret_cast<int>(packet->data) & AVIO_FFMPEG_ALIGN_MASK ) {
            alignPacket(packet);
        }

        // czy to ten strumieñ?
        if ( packet->stream_index == videoStream->index ) {
            // pomocnicza zmienna
            codecContext->reordered_opaque = packet->pts;

#ifdef VIDLIB_FFMPEG_ENABLE_NEWAPI
            //packet->flags |= PKT_FLAG_KEY;
            {
                error = avcodec_decode_video2(codecContext, frame, &gotPicture, packet);
                if ( error < 0 ) {
                    // tolerujemy b³¹d
                }
            }
            // czy uda³o siê?
            if ( gotPicture ) {
                // obliczenie timestamp'u
                frameTimestamp = getTimestamp(frame, packet->dts);
                nextFrameTimestamp = std::min<int64_t>(frameTimestamp+frameSpan, videoStream->duration);
                onGotFrame(frame, frameTimestamp);
                return true;
            }

#else // VIDLIB_FFMPEG_ENABLE_NEWAPI

            frameBytesRemaining = packet->size;
            frameData = packet->data;

#endif // VIDLIB_FFMPEG_ENABLE_NEWAPI
        }
    }
}

//------------------------------------------------------------------------------

int64_t FFmpegVideoStream::getTimestamp( AVFrame * pFrame, int64_t packetDTS )
{
    VIDLIB_FUNCTION_PROLOG;
    int64_t timestamp = 0;
    if ( packetDTS == AV_NOPTS_VALUE && pFrame->reordered_opaque != AV_NOPTS_VALUE ) {
        timestamp = pFrame->reordered_opaque;
    } else if ( packetDTS != AV_NOPTS_VALUE ) {
        timestamp = packetDTS;
    }
    return timestamp;// * av_q2d(videoStream->time_base);
}

//------------------------------------------------------------------------------

inline double FFmpegVideoStream::timestampToSec( int64_t timestamp ) const
{
    VIDLIB_FUNCTION_PROLOG;
    UTILS_ASSERT(videoStream);
    return timestamp * av_q2d(videoStream->time_base);
}

//------------------------------------------------------------------------------

inline int64_t FFmpegVideoStream::secToTimestap( double time ) const
{
    VIDLIB_FUNCTION_PROLOG;
    UTILS_ASSERT(videoStream);
    return static_cast<int64_t>(time * videoStream->time_base.den / videoStream->time_base.num);
}

//------------------------------------------------------------------------------

inline bool FFmpegVideoStream::isKeyframe() const
{
    VIDLIB_FUNCTION_PROLOG;
    UTILS_ASSERT(frame);
    return frame->key_frame != 0;
}

//------------------------------------------------------------------------------

void FFmpegVideoStream::alignPacket( AVPacket * packet )
{
    VIDLIB_FUNCTION_PROLOG;
    UTILS_ASSERT(packet && alignedPacket);
    int realSize = packet->size + VIDLIB_FFMPEG_ALIGN_BYTES;
    if ( realSize > alignedPacket->size ) {
        // alokujemy pamiêæ dla pakietu...
        av_free_packet(alignedPacket);
        av_new_packet(alignedPacket, realSize);
    }

    // alignujemy wskaŸnik i rozmiar ...
    uint8_t * alignedData = reinterpret_cast<uint8_t*>(reinterpret_cast<int>(alignedPacket->data + AVIO_FFMPEG_ALIGN_MASK) & (~AVIO_FFMPEG_ALIGN_MASK));
    int alignedSize = alignedPacket->size - (alignedData - alignedPacket->data);

    // kopiujemy dane
    ::memcpy(alignedData, packet->data, packet->size);
    AVPacket packetCopy = *packet;

    // zwalniamy
    av_free_packet(packet);

    // i przywracamy ustawienia
    *packet = packetCopy;
    // nie pozwalamy na kasowanie!
    packet->destruct = NULL;
    packet->data = alignedData;
    // shrink spowoduje, ¿e dodatkowe miejsce alokowane przez ffmpega bêdzie wyzerowane
    packet->size = alignedSize;
    av_shrink_packet(packet, packetCopy.size);
}

//------------------------------------------------------------------------------

bool FFmpegVideoStream::skipFramesToTimestamp( int64_t targetTimestamp, int maxFramesToSkip, bool stopOnKeyframe )
{
    VIDLIB_FUNCTION_PROLOG;
    //Measurer measure("skipFramesToTimestamp");

    int64_t delta = targetTimestamp - frameTimestamp;
    UTILS_ASSERT(frameTimestamp != AV_NOPTS_VALUE);
    UTILS_ASSERT(delta > 0, "Can't skip backwards.");
    UTILS_ASSERT(delta <= maxFramesToSkip * frameSpan, "Target timestamp too distant.");

    int nCount = maxFramesToSkip + 1;
    while ( --nCount && nextFrameTimestamp <= targetTimestamp && nextFrameTimestamp != videoStream->duration ) {
        if ( !readFrame() ) {
            VIDLIB_ERROR( *getLastError() );
        }
        if ( isKeyframe() ) {
            if ( stopOnKeyframe ) {
                return true;
            } else {
                UTILS_DEBUG_PRINT("Skipped as %d from %d%", maxFramesToSkip-nCount, maxFramesToSkip);
            }
        }
    }
    return true;
}

//------------------------------------------------------------------------------

void FFmpegVideoStream::onGotFrame(AVFrame * frame, int64_t timestamp)
{
    VIDLIB_FUNCTION_PROLOG;
    // czy to najwiêkszy odstêp?
    int64_t span = timestamp - keyframeStats.lastTimestamp;
    if ( keyframeStats.lastTimestamp != AV_NOPTS_VALUE ) {
        keyframeStats.maxSpan = std::max<int64_t>(span, keyframeStats.maxSpan);
    }

    // czy to ramka kluczowa?
    if ( frame->key_frame ) {
        if ( keyframeStats.lastTimestamp != AV_NOPTS_VALUE && timestamp != keyframeStats.lastTimestamp ) {
            // ok, zwiêkszamy licznik
            unsigned int prevCount = keyframeStats.count++;
            // aktualizujemy œredni¹
            double spanSec = timestampToSec(span);
            keyframeStats.avgSpan = (keyframeStats.avgSpan * prevCount + spanSec) / keyframeStats.count;
        }
        keyframeStats.lastTimestamp = timestamp;
    }

    onFrameRead();
}

//------------------------------------------------------------------------------

bool FFmpegVideoStream::getData( PictureLayered & dst )
{
    VIDLIB_FUNCTION_PROLOG;
    utils::zero(dst);
    dst.format = static_cast<PixelFormat>(codecContext->pix_fmt);
    dst.width = codecContext->width;
    dst.height = codecContext->height;
    //
    for (int i = 0; i < 4; ++i) {
        dst.data[i] = frame->data[i];
        dst.dataWidth[i] = frame->linesize[i];
        dst.dataHeight[i] = dst.dataWidth[i] ? codecContext->height : 0;
    }

    // dla YV12 zmieniamy wysokoœæ...
    if ( codecContext->pix_fmt == PIX_FMT_YUV420P ) {
        dst.dataHeight[1]>>=1;
        dst.dataHeight[2]>>=1;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
} // namespace vidlib
////////////////////////////////////////////////////////////////////////////////
