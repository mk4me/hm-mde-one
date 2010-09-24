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
 *	Strumie� wej�ciowy video kontorlowany przez bibliotek� ffmpeg.
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

  //! Statystki dotycz�ce klatek kluczowych. S�u�� do optymalizacji operacji
  //! przesuwania si� w strumieniu.
  struct KeyframeStats {
    //! Liczba przetworzonych klatek kluczowych.
    unsigned int count;
    //! Czas ostatniej ramki kluczowej. W jednostkach strumienia.
    int64_t lastTimestamp;
    //! �redni odst�p mi�dzy ramkami kluczowymi.
    double avgSpan;
    //! Maksymalny odst�p mi�dzy klatkami kluczowymi.
    int64_t maxSpan;
  };

//------------------------------------------------------------------------------
private:
  //! 
  AVFormatContext * formatContext;
  //! Kodek video.
  AVCodecContext * codecContext;
  //! Strumie�.
  AVStream * videoStream;
  //! Bie��ca ramka.
  AVFrame * frame;
  //! Prawdziwy timestamp prezentacji bie��cej ramki. W jednostkach strumienia.
  int64_t frameTimestamp;
  //! Minimalny odst�p mi�dzy ramkami.
  int64_t frameSpan;
  //! Timestamp nast�pnej ramki.
  int64_t nextFrameTimestamp;

  //! Czas w �r�dle widziany na zewn�trz klasy. Mo�e by� >= frameTimestamp,
  //! ale tylko je�li wci�� dotyczy tej samej klatki.
  double wantedTime;
  //! Statystyki klatek kluczowych.
  KeyframeStats keyframeStats;

  //! Liczba bajt�w do zdekodowania. U�ywane tylko w starym API.
  int frameBytesRemaining;
  //! Dane pakietu. Tylko w starym API.
  const uint8_t * frameData;
  //! Pakiet.
  AVPacket * packet;
  //! Prawdziwy u�ywany w momencie, gdy �r�d�owy nie ma wyalignowanych danych.
  AVPacket * alignedPacket;

//------------------------------------------------------------------------------
public:
  //! \param source �r�d�o.
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
  //! \param time Pozycja w �r�dle.
  virtual bool setTime(double time);

  //! \param callback
  static void setLockManager(LockManager callback);

//------------------------------------------------------------------------------
protected:
  //! Je�eli strumie� jest w stanie zaprezentowa� ramk� w postaci zdj�cia
  //! powinien prze�adowa� t� metod�.
  //! \param dst
  virtual bool getData(PictureLayered & dst);

//------------------------------------------------------------------------------
private:
  //! Pomija ramki a� dotrze do zadanego czasu.
  //! Je�li po wywo�aniu tej funkcji warunek frameTimestamp >= targetTimestamp nie jest spe�niony
  //! strumie� znajduje si� w nieokre�lonym stanie i trzeba r�cznie ustawi� jego pozycj� metod�
  //! seekToKeyframe.
  bool skipFramesToTimestamp( int64_t targetTimestamp, int maxFramesToSkip, bool stopOnKeyframe );
  //! Przechodzi do wybranego miejsca w strumieniu.
  bool seekToKeyframe(int64_t timestamp, bool pickNextFrame);
  //! Odczytuje klatk� z bie��cej pozycji w strumieniu.
  bool readFrame();
  //!
  void alignPacket(AVPacket * packet);
  //!
  int64_t getTimestamp( AVFrame * pFrame, int64_t packetDTS );
  //!
  inline double timestampToSec( int64_t timestamp ) const;
  //!
  inline int64_t secToTimestap( double time ) const;
  //! Czy bie��ca klatka jest kluczowa?
  inline bool isKeyframe() const;

  //!
  bool init(const std::string& source, int wantedVideoStream = -1);
  //! Wewn�trzny callback wywo�ywany w momencie kiedy uda si� zdekodowa� ramk�.
  void onGotFrame(AVFrame * frame, int64_t timestamp);
};

////////////////////////////////////////////////////////////////////////////////
} // namespace vm
////////////////////////////////////////////////////////////////////////////////

#endif  // __AVINOUT_FFMPEGVIDEOINPUT_H__