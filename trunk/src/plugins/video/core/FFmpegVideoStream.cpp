#include "../VideoPCH.h"
#include "VMPrivate.h"
#include "FFmpegError.h"
#include "FFmpegVideoStream.h"

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
#endif

// "e:\Filmy\The Simpsons\21x01 - Homer the Whopper.avi" --window 10 10 200 200

extern "C" {
#pragma warning (push)
#pragma warning (disable: 4244)
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/mathematics.h>
#pragma warning (pop)
}

//! Ile ramek mo¿na omin¹æ podczas wyszukiwania do przodu?
#define AVIO_FFMPEG_MAXSKIPFRAMES 100
//! Ile ramek mo¿na omin¹æ podczas wyszukiwania do przodu?
#define AVIO_FFMPEG_MINSKIPFRAMES 5
//! Maska u¿ywana podczas alignowania danych
#define AVIO_FFMPEG_ALIGN_MASK (VM_FFMPEG_ALIGN_BYTES-1)


#define AVIO_FFMPEG_SEEK_WITH_STREAM_INDEX
//#define AVIO_FFMPEG_ENABLE_EXPERIMENTAL_API


#define BREAK_ON_ERROR( x ) { if (!(x)) { VM_ERROR(*getLastError()); }}


//! Wyzerowany callback.
static video::FFmpegVideoStream::LockManager lockManager = NULL;

//! Funkcja przekazuj¹ca ¿¹danie locka do ustawionego callbacka.
//! \param mutex
//! \param op
static int FFmpegLockForwarder( void **mutex, enum AVLockOp op )
{
  using namespace video;
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

////////////////////////////////////////////////////////////////////////////////
namespace video {
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

FFmpegVideoStream::FFmpegVideoStream( const std::string& source, int wantedVideoStream /*= -1*/ )
: VideoStream(source)
{
  VM_FUNCTION_PROLOG;
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
  //UTILS_ASSERT(frameCount==getFrameCount(), "Frame count does not match (%d vs %d)", frameCount, getFrameCount());
}

//------------------------------------------------------------------------------

FFmpegVideoStream::~FFmpegVideoStream()
{
  VM_FUNCTION_PROLOG;
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
  VM_FUNCTION_PROLOG;
  int error = 0;
  // mo¿na inicjalizowaæ wiele razy, sprawdza wewnetrznie czy ju¿ by³a zainicjalizowana
  av_register_all();
  // otwieramy plik
  if (error = av_open_input_file(&formatContext, source.c_str(), NULL, 0, NULL)) {
    VM_ERROR(FFmpegError( "av_open_input_file error", error ));
  }
  // info o kodekach
  if ( (error=av_find_stream_info(formatContext)) < 0 ) {
    VM_ERROR(FFmpegError( "av_find_stream_info error", error ));
  }

#ifdef VM_DEBUG
  dump_format(formatContext, 0, source.c_str(), false);
#endif // VM_DEBUG

  // szukamy strumienia video
  for (unsigned int i = 0; i < formatContext->nb_streams; ++i) {
    AVStream * selectedStream = formatContext->streams[i];
    if ( selectedStream->codec->codec_type == CODEC_TYPE_VIDEO ) {
      // czy to jest ten strumieñ, który chcemy otworzyæ?
      if ( wantedVideoStream < 0 || wantedVideoStream == i ) {
        codecContext = selectedStream->codec;
        videoStream = selectedStream;
        break;
      }
    }
  }

  // TODO: ujednolicenie errorów
  if ( codecContext == NULL ) {
    VM_ERROR( FFmpegError( "Video stream not found." ) );
  }

  // pobieramy kodek
  AVCodec *pCodec = avcodec_find_decoder(codecContext->codec_id);
  if(pCodec==NULL) {
    VM_ERROR( FFmpegError( "avcodec_find_decoder" ) );
  }

  // otwieramy kodek
  if( (error=avcodec_open(codecContext, pCodec)) <0) {
    VM_ERROR( FFmpegError( "avcodec_open", error ) );
  }

  // alokacja kaltki
  frame = avcodec_alloc_frame();
  if ( !frame ) {
    VM_ERROR( FFmpegError( "avcodec_alloc_frame error", error ) );
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
  PixelFormat format = static_cast<video::PixelFormat>(codecContext->pix_fmt);

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
  VM_FUNCTION_PROLOG;
  if ( frameTimestamp == AV_NOPTS_VALUE ) {
    if ( !setTime(0.0) ) {
      VM_ERROR( *getLastError() );
    }
  } else if ( !readFrame() ) {
    VM_ERROR(*getLastError());
  }
  wantedTime = timestampToSec(frameTimestamp);
  return true;
}

//------------------------------------------------------------------------------

double FFmpegVideoStream::getTime() const
{
  VM_FUNCTION_PROLOG;
  return wantedTime;
}

//------------------------------------------------------------------------------

bool FFmpegVideoStream::setTime( double time )
{
  VM_FUNCTION_PROLOG;
  //Measurer measurer("setTime");
  if ( time < 0.0 || time > getDuration() ) {
    // przy debugowaniu mo¿na zakomentowaæ ten b³¹d i wyzerowaæ czas
    //VM_ERROR( FFmpegError("time < 0.0 || time > getDuration()") );
    time = 0.0;
  }
  if ( time == getDuration() ) {
    // TODO
    // ffmpeg Ÿle ustawia siê przy seeku do zerowego czasu (wypluwa ostatni¹ odczytan¹ ramkê)
    // tymczasowe, do stawiania breakpointów jeœli czas == 0
    int brr = 0;
  }

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
    int repeatCount = 3;
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
        VM_ERROR( FFmpegError("Error during seek.") );
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
  VM_FUNCTION_PROLOG;
  return timestampToSec(frameTimestamp);
}

//------------------------------------------------------------------------------

double FFmpegVideoStream::getNextFrameTimestamp() const
{
  VM_FUNCTION_PROLOG;
  return timestampToSec(frameTimestamp+frameSpan);
}

//------------------------------------------------------------------------------

bool FFmpegVideoStream::seekToKeyframe( int64_t timestamp, bool pickNextframe )
{
  VM_FUNCTION_PROLOG;
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
      VM_ERROR( FFmpegError("Error during seek.", error) );
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
    VM_ERROR( *getLastError() );
  }
  return false;
}

//------------------------------------------------------------------------------

bool FFmpegVideoStream::readFrame()
{
  VM_FUNCTION_PROLOG;
  //Measurer measure("readFrame");
  int error = 0;
  int gotPicture = 0;

  while (true) {

#ifndef VM_FFMPEG_ENABLE_NEWAPI
    //!
    while ( frameBytesRemaining > 0 ) {
      // odczytujemy ramkê
      const int bytesDecoded = avcodec_decode_video(codecContext, frame, &gotPicture, frameData, frameBytesRemaining);
      if ( bytesDecoded < 0 ) {
        VM_ERROR( FFmpegError("Error decoding frame") );
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
#endif // VM_FFMPEG_ENABLE_NEWAPI

    // czy to koniec?
    if ( url_feof(formatContext->pb) ) {
      nextFrameTimestamp = frameTimestamp = videoStream->duration;
      return true;
      //VM_ERROR( FFmpegError("End of videoStream reached.") );
    }

    // zwalniamy poprzedni pakiet
    av_free_packet(packet);

    // odczytujemy pakiet
    {
      error = av_read_frame(formatContext, packet);
      if ( error < 0 ) {
        if ( error == AVERROR_EOF ) {
          //VM_ERROR( FFmpegError("Unexpected EOF", error));
          nextFrameTimestamp = frameTimestamp = videoStream->duration;
          return true;
        } else {
          VM_ERROR( FFmpegError("Error reading frame", error) );
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

#ifdef VM_FFMPEG_ENABLE_NEWAPI
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

#else // VM_FFMPEG_ENABLE_NEWAPI

      frameBytesRemaining = packet->size;
      frameData = packet->data;

#endif // VM_FFMPEG_ENABLE_NEWAPI
    }
  }
  return false;
}

//------------------------------------------------------------------------------

int64_t FFmpegVideoStream::getTimestamp( AVFrame * pFrame, int64_t packetDTS )
{
  VM_FUNCTION_PROLOG;
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
  VM_FUNCTION_PROLOG;
  UTILS_ASSERT(videoStream);
  return timestamp * av_q2d(videoStream->time_base);
}

//------------------------------------------------------------------------------

inline int64_t FFmpegVideoStream::secToTimestap( double time ) const
{
  VM_FUNCTION_PROLOG;
  UTILS_ASSERT(videoStream);
  return static_cast<int64_t>(time * videoStream->time_base.den / videoStream->time_base.num);
}

//------------------------------------------------------------------------------

inline bool FFmpegVideoStream::isKeyframe() const
{
  VM_FUNCTION_PROLOG;
  UTILS_ASSERT(frame);
  return frame->key_frame != 0;
}

//------------------------------------------------------------------------------

void FFmpegVideoStream::alignPacket( AVPacket * packet )
{
  VM_FUNCTION_PROLOG;
  UTILS_ASSERT(packet && alignedPacket);
  int realSize = packet->size + VM_FFMPEG_ALIGN_BYTES;
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
  VM_FUNCTION_PROLOG;
  //Measurer measure("skipFramesToTimestamp");

  int64_t delta = targetTimestamp - frameTimestamp;
  UTILS_ASSERT(frameTimestamp != AV_NOPTS_VALUE);
  UTILS_ASSERT(delta > 0, "Can't skip backwards.");
  UTILS_ASSERT(delta <= maxFramesToSkip * frameSpan, "Target timestamp too distant.");

  int nCount = maxFramesToSkip + 1;
  while ( --nCount && nextFrameTimestamp <= targetTimestamp && nextFrameTimestamp != videoStream->duration ) {
    if ( !readFrame() ) {
      VM_ERROR( *getLastError() );
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
  VM_FUNCTION_PROLOG;
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

void FFmpegVideoStream::setLockManager( LockManager manager )
{
  lockManager = manager;
}

//------------------------------------------------------------------------------

bool FFmpegVideoStream::getData( PictureLayered & dst )
{
  VM_FUNCTION_PROLOG;
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
} // namespace video
////////////////////////////////////////////////////////////////////////////////
