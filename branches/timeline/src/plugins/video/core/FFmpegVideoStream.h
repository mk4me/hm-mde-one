/********************************************************************
	created:  2010/02/11
	created:  11:2:2010   13:14
	filename: FFmpegVideoStream.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __AVINOUT_FFMPEGVIDEOINPUT_H__
#define __AVINOUT_FFMPEGVIDEOINPUT_H__

struct AVFormatContext;
struct AVCodecContext;
struct AVFrame;
struct AVStream;
struct AVPacket;
struct AVPicture;

////////////////////////////////////////////////////////////////////////////////
namespace vm {
////////////////////////////////////////////////////////////////////////////////

/**
 *	Strumieñ wejœciowy video kontorlowany przez bibliotekê ffmpeg.
 */
class FFmpegVideoStream : public VideoStream
{
//------------------------------------------------------------------------------
public:
  //!
  enum LockOp {
    LockOpCreate,
    LockOpDestroy,
    LockOpRelease,
    LockOpObtain
  };

  //!
  typedef int (*LockManager)(void **mutex, LockOp lock);

//------------------------------------------------------------------------------
private:
  //! Pomocnicza klasa do inicjalizacji ffmpega.
  class Initializer
  {
  public:
    Initializer();
    ~Initializer();
  };

  //! Statystki dotycz¹ce klatek kluczowych. S³u¿¹ do optymalizacji operacji
  //! przesuwania siê w strumieniu.
  struct KeyframeStats {
    //! Liczba przetworzonych klatek kluczowych.
    unsigned int count;
    //! Czas ostatniej ramki kluczowej. W jednostkach strumienia.
    int64_t lastTimestamp;
    //! Œredni odstêp miêdzy ramkami kluczowymi.
    double avgSpan;
    //! Maksymalny odstêp miêdzy klatkami kluczowymi.
    int64_t maxSpan;
  };

//------------------------------------------------------------------------------
private:
  //! 
  AVFormatContext * formatContext;
  //! Kodek video.
  AVCodecContext * codecContext;
  //! Strumieñ.
  AVStream * videoStream;
  //! Bie¿¹ca ramka.
  AVFrame * frame;
  //! Prawdziwy timestamp prezentacji bie¿¹cej ramki. W jednostkach strumienia.
  int64_t frameTimestamp;
  //! Minimalny odstêp miêdzy ramkami.
  int64_t frameSpan;
  //! Timestamp nastêpnej ramki.
  int64_t nextFrameTimestamp;

  //! Czas w Ÿródle widziany na zewn¹trz klasy. Mo¿e byæ >= frameTimestamp,
  //! ale tylko jeœli wci¹¿ dotyczy tej samej klatki.
  double wantedTime;
  //! Statystyki klatek kluczowych.
  KeyframeStats keyframeStats;

  //! Liczba bajtów do zdekodowania. U¿ywane tylko w starym API.
  int frameBytesRemaining;
  //! Dane pakietu. Tylko w starym API.
  const uint8_t * frameData;
  //! Pakiet.
  AVPacket * packet;
  //! Prawdziwy u¿ywany w momencie, gdy Ÿród³owy nie ma wyalignowanych danych.
  AVPacket * alignedPacket;

//------------------------------------------------------------------------------
public:
  //! \param source ród³o.
  FFmpegVideoStream(const std::string& source, int wantedVideoStream = -1);
  //!
  virtual ~FFmpegVideoStream();

//------------------------------------------------------------------------------
public:
  

  //!
  virtual bool readNext();
  //! 
  virtual double getFrameTimestamp() const;
  //!
  virtual double getNextFrameTimestamp() const;
  //! \retrun Pozycja w strumieniu.
  virtual double getTime() const;
  //! \param time Pozycja w Ÿródle.
  virtual bool setTime(double time);

  //! \param callback
  static void setLockManager(LockManager callback);

//------------------------------------------------------------------------------
protected:
  //! Je¿eli strumieñ jest w stanie zaprezentowaæ ramkê w postaci zdjêcia
  //! powinien prze³adowaæ tê metodê.
  //! \param dst
  virtual bool getData(PictureLayered & dst);

//------------------------------------------------------------------------------
private:
  //! Pomija ramki a¿ dotrze do zadanego czasu.
  //! Jeœli po wywo³aniu tej funkcji warunek frameTimestamp >= targetTimestamp nie jest spe³niony
  //! strumieñ znajduje siê w nieokreœlonym stanie i trzeba rêcznie ustawiæ jego pozycjê metod¹
  //! seekToKeyframe.
  bool skipFramesToTimestamp( int64_t targetTimestamp, int maxFramesToSkip, bool stopOnKeyframe );
  //! Przechodzi do wybranego miejsca w strumieniu.
  bool seekToKeyframe(int64_t timestamp, bool pickNextFrame);
  //! Odczytuje klatkê z bie¿¹cej pozycji w strumieniu.
  bool readFrame();
  //!
  void alignPacket(AVPacket * packet);
  //!
  int64_t getTimestamp( AVFrame * pFrame, int64_t packetDTS );
  //!
  inline double timestampToSec( int64_t timestamp ) const;
  //!
  inline int64_t secToTimestap( double time ) const;
  //! Czy bie¿¹ca klatka jest kluczowa?
  inline bool isKeyframe() const;

  //!
  bool init(const std::string& source, int wantedVideoStream = -1);
  //! Wewnêtrzny callback wywo³ywany w momencie kiedy uda siê zdekodowaæ ramkê.
  void onGotFrame(AVFrame * frame, int64_t timestamp);
};

////////////////////////////////////////////////////////////////////////////////
} // namespace vm
////////////////////////////////////////////////////////////////////////////////

#endif  // __AVINOUT_FFMPEGVIDEOINPUT_H__