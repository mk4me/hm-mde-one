#ifndef HEADER_GUARD_OSGFFMPEG_VIDEO_IMAGE_STREAM_H
#define HEADER_GUARD_OSGFFMPEG_VIDEO_IMAGE_STREAM_H

#include <osg/ImageStream>
#include <OpenThreads/Thread>
#include <osg/Timer>

#include <utils/PtrWrapper.h>
#include <utils/PtrPolicyOSG.h>

#include "../core/VM.h"

namespace vmOSGPlugin
{

/**
 *	Strumieñ wideo.
 */
class VideoImageStream : public osg::ImageStream, public OpenThreads::Thread
{
protected:
  //! Typ wskaŸnika do strumienia.
  typedef utils::PtrWrapper<vm::VideoStream, utils::PtrPolicyOSG> Stream;
  //! Typ blokady opartej na muteksie.
  typedef OpenThreads::ScopedLock<OpenThreads::Mutex> ScopedLock;
  //! Wewnêtrzy strumieñ.
  osg::ref_ptr<Stream> innerStream; 
  /** Mutex */
  OpenThreads::Mutex streamStateMutex;
  /** Timer */
  osg::Timer timer;
  /** Skala czasowa */
  double timeScale;
  /** Poprzednia pozycja w strumieniu */
  double prevTimestamp;
  /** Format piksela */
  vm::PixelFormat targetFormat;
  /** Bufor na bie¿¹c¹ ramkê */
  // vm::FrameData frameData;
  vm::Picture currentPicture;
  //! Maksymalna szerokoœæ.
  int maxWidth;

public:
  /** Konstruktor zeruj¹cy */
  VideoImageStream();
  /** Konstruktor kopiuj¹cy */
  VideoImageStream(const VideoImageStream & image, const osg::CopyOp & copyop = osg::CopyOp::SHALLOW_COPY);
  /** Domyœlne implementacje */
  META_Object(vmOSGPlugin, VideoImageStream);

  /** Odtwarzanie strumienia (nowy w¹tek!) */
  virtual void play();
  /** Zatrzymanie strumienia */
  virtual void pause();
  /** Odtwarzanie "do ty³u" */
  virtual void rewind();
  /** Wyszukiwanie w strumieniu */
  virtual void seek(double time);
  /** Zamkniêcie strumienia */
  virtual void quit(bool waitForThreadToExit = true);

  /** Czas utworzenia */
  virtual double getCreationTime() const;
  /** D³ugoœæ strumienia */
  virtual double getLength() const;
  /** Liczba klatek na sekundê */
  virtual double getFrameRate() const;
  /** Czas w strumieniu */
  virtual double getReferenceTime() const;
  /** Czas w strumieniu */
  virtual void setReferenceTime(double value) { seek(value); }
  /** Skala czasowa */
  virtual void setTimeMultiplier(double timescale);
  /** Skala czasowa */
  virtual double getTimeMultiplier() const;

  /** Czy wideo ma kana³ alpha? */
  virtual bool isImageTranslucent() const;

  /** Otwiera strumieñ */
  virtual bool open(const std::string & filename);

  //!
  virtual bool requiresUpdateCall() const;
  //!
  virtual void update(osg::NodeVisitor* nv);


  //! \return Format do którego film jest konwertowany.
  inline vm::PixelFormat getTargetFormat() const
  {
    return targetFormat;
  }
  //! \return Maksymalna szerokoœæ konwertowanego strumienia.
  inline int getMaxWidth() const
  { 
    return maxWidth;
  }
  //! \param maxWidth Maksymalna szerokoœæ konwertowanego strumienia.
  void setMaxWidth(int maxWidth);

protected:
  /** */
  virtual ~VideoImageStream();
  /** W¹tek odtwarzaj¹cy */
  virtual void run();
  /** Tryb zapêtlania */
  virtual void applyLoopingMode();
  /** Ustawia bie¿¹c¹ ramkê (jeœli jest nowsza ni¿ poprzednia)*/
  virtual void publishFrame();
  /** Ustawia bie¿¹cy czas */
  virtual void setStreamTime( double time );
  /** Ustawia bie¿¹c¹ ramkê */
  void reloadImage(vm::Picture& picture);

  //! \return Wewnêtrzny strumieñ.
  inline vm::VideoStream* getStream()
  {
    return *(innerStream.get());
  }
  //! \return Wewnêtrzny strumieñ.
  inline const vm::VideoStream* getStream() const
  {
    return *(innerStream.get());
  }

};

}



#endif // HEADER_GUARD_OSGFFMPEG_VIDEO_IMAGE_STREAM_H