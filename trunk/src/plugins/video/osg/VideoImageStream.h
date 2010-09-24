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
 *	Strumie� wideo.
 */
class VideoImageStream : public osg::ImageStream, public OpenThreads::Thread
{
protected:
  //! Typ wska�nika do strumienia.
  typedef utils::PtrWrapper<vm::VideoStream, utils::PtrPolicyOSG> Stream;
  //! Typ blokady opartej na muteksie.
  typedef OpenThreads::ScopedLock<OpenThreads::Mutex> ScopedLock;
  //! Wewn�trzy strumie�.
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
  /** Bufor na bie��c� ramk� */
  // vm::FrameData frameData;
  vm::Picture currentPicture;
  //! Maksymalna szeroko��.
  int maxWidth;

public:
  /** Konstruktor zeruj�cy */
  VideoImageStream();
  /** Konstruktor kopiuj�cy */
  VideoImageStream(const VideoImageStream & image, const osg::CopyOp & copyop = osg::CopyOp::SHALLOW_COPY);
  /** Domy�lne implementacje */
  META_Object(vmOSGPlugin, VideoImageStream);

  /** Odtwarzanie strumienia (nowy w�tek!) */
  virtual void play();
  /** Zatrzymanie strumienia */
  virtual void pause();
  /** Odtwarzanie "do ty�u" */
  virtual void rewind();
  /** Wyszukiwanie w strumieniu */
  virtual void seek(double time);
  /** Zamkni�cie strumienia */
  virtual void quit(bool waitForThreadToExit = true);

  /** Czas utworzenia */
  virtual double getCreationTime() const;
  /** D�ugo�� strumienia */
  virtual double getLength() const;
  /** Liczba klatek na sekund� */
  virtual double getFrameRate() const;
  /** Czas w strumieniu */
  virtual double getReferenceTime() const;
  /** Czas w strumieniu */
  virtual void setReferenceTime(double value) { seek(value); }
  /** Skala czasowa */
  virtual void setTimeMultiplier(double timescale);
  /** Skala czasowa */
  virtual double getTimeMultiplier() const;

  /** Czy wideo ma kana� alpha? */
  virtual bool isImageTranslucent() const;

  /** Otwiera strumie� */
  virtual bool open(const std::string & filename);

  //!
  virtual bool requiresUpdateCall() const;
  //!
  virtual void update(osg::NodeVisitor* nv);


  //! \return Format do kt�rego film jest konwertowany.
  inline vm::PixelFormat getTargetFormat() const
  {
    return targetFormat;
  }
  //! \return Maksymalna szeroko�� konwertowanego strumienia.
  inline int getMaxWidth() const
  { 
    return maxWidth;
  }
  //! \param maxWidth Maksymalna szeroko�� konwertowanego strumienia.
  void setMaxWidth(int maxWidth);

protected:
  /** */
  virtual ~VideoImageStream();
  /** W�tek odtwarzaj�cy */
  virtual void run();
  /** Tryb zap�tlania */
  virtual void applyLoopingMode();
  /** Ustawia bie��c� ramk� (je�li jest nowsza ni� poprzednia)*/
  virtual void publishFrame();
  /** Ustawia bie��cy czas */
  virtual void setStreamTime( double time );
  /** Ustawia bie��c� ramk� */
  void reloadImage(vm::Picture& picture);

  //! \return Wewn�trzny strumie�.
  inline vm::VideoStream* getStream()
  {
    return *(innerStream.get());
  }
  //! \return Wewn�trzny strumie�.
  inline const vm::VideoStream* getStream() const
  {
    return *(innerStream.get());
  }

};

}



#endif // HEADER_GUARD_OSGFFMPEG_VIDEO_IMAGE_STREAM_H