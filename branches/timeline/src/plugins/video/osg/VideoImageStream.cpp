#include "../VideoPCH.h"
#include <iostream>

#include <osg/Notify>
#include <utils/Align.h>

#include "../core/VMPrivate.h"
#include "VideoImageStream.h"

namespace vmOSGPlugin {

//------------------------------------------------------------------------------

VideoImageStream::VideoImageStream() :
innerStream(NULL), timeScale(1.0), prevTimestamp(vm::INVALID_TIMESTAMP), streamStateMutex(), 
maxWidth(-1)
{
  VM_FUNCTION_PROLOG;
  //utils::zero(frameData);
  targetFormat = vm::VideoManager::getInstance()->getPrefferedFormat();
  //frameData.format = targetFormat;
  utils::zero(currentPicture);
  currentPicture.format = targetFormat;
  setOrigin(osg::Image::TOP_LEFT);

  
}

VideoImageStream::VideoImageStream(const VideoImageStream & image, const osg::CopyOp & copyop) :
osg::ImageStream(image, copyop), targetFormat(image.targetFormat), maxWidth(image.maxWidth)
{
  VM_FUNCTION_PROLOG;
  //utils::zero(frameData);
  //frameData.format = targetFormat;
  utils::zero(currentPicture);
  currentPicture.format = targetFormat;
  // TODO: nie bêdzie dzia³aæ!
  // copy input decoder
  innerStream = image.innerStream;
//  if ( copyop != osg::CopyOp::SHALLOW_COPY ) {
//    OSG_FATAL << "not implemented" << std::endl;
//    assert(false);
//  }
}

VideoImageStream::~VideoImageStream()
{
  VM_FUNCTION_PROLOG;
  OSG_INFO<<"Destructing VideoImageStream..."<<std::endl;

  // quit
  quit(true);

  OSG_INFO<<"Have done quit"<<std::endl;

  // release athe audio streams to make sure that the decoder doesn't retain any external
  // refences.
  getAudioStreams().clear();

  //utils::deletePtr(innerStream);

  OSG_INFO<<"Destructed VideoImageStream."<<std::endl;
}

//------------------------------------------------------------------------------

void VideoImageStream::run()
{
  try
  {
    // pocz¹tek
    {
      ScopedLock lock(streamStateMutex);
      // synchronizacja timera
      setStreamTime(getStream()->getTime());
    }

    bool done = false;
    osg::Timer frameLength;

    while ( _status == PLAYING && !done ) {
      frameLength.setStartTick();
      {
        // aktualizacja
        ScopedLock lock(streamStateMutex);

        // bie¿¹ce tykniêcie
        osg::Timer_t tick = timer.tick();
        double delta = timer.delta_s(timer.getStartTick(), tick);
        // ustawiamy czas
        setStreamTime( getStream()->getTime() + delta * timeScale );
        // "poprawiamy" timer
        timer.setStartTick(tick);
        publishFrame();

        if ( getStream()->isEndOfStream() ) {
          done = (_loopingMode != LOOPING);
        }
      }
      // jak d³ugo to trwa³o?
      double waitTime = getStream()->getFrameDuration() / timeScale - frameLength.time_s();
      if ( waitTime > 0.0 ) {
        // czekamy
        OpenThreads::Thread::microSleep( waitTime * 1000000 );
      }
    }
  } catch (const std::exception & error) {
    OSG_WARN << "VideoImageStream::run : " << error.what() << std::endl;
  } catch (...) {
    OSG_WARN << "VideoImageStream::run : unhandled exception" << std::endl;
  }
  OSG_NOTICE<<"Finished VideoImageStream::run()"<<std::endl;
}

//------------------------------------------------------------------------------

bool VideoImageStream::open(const std::string &filename)
{
  VM_FUNCTION_PROLOG;
  // nazwa (czemu?)
  setFileName(filename);

  // try open file
  try {
    //innerStream = new VideoAdapter(filename);
    innerStream = new Stream(vm::VideoManager::getInstance()->createStream(filename));
  } catch ( vm::VideoError & err ) {
    // failed!
    OSG_NOTICE<<"ffmpeg::open("<<filename<<") reported an error: "<< err.what() << std::endl;
    return false;
  }


  // ustawiamy zerowy czas
  setStreamTime(0.0);
  setMaxWidth(maxWidth);
  prevTimestamp = getStream()->getFrameTimestamp();

  //currentPicture.free();
  //currentPicture = vm::Picture::create(getStream()->getWidth(), getStream()->getHeight(), targetFormat);


  
  //publishFrame();

  // aspect ratio
  if ( targetFormat == vm::PixelFormatYV12 ) {
    // poniewa¿ wymagamy konwersji w shaderach, ustawiamy aspect ratio
    setPixelAspectRatio( static_cast<float>(getStream()->getAspectRatio()) * 1.5f );
  } else {
    setPixelAspectRatio( static_cast<float>(getStream()->getAspectRatio()));
  }

  // info
  OSG_NOTICE<<"ffmpeg::open("<<filename<<") size("<<s()<<", "<<t()<<") aspect ratio "<<getPixelAspectRatio()<<std::endl;
  OSG_NOTICE<<"\tinner format: "<<utils::Enum<vm::PixelFormat>::getName(getStream()->getPixelFormat())<<std::endl;
  OSG_NOTICE<<"\tstream format: "<<utils::Enum<vm::PixelFormat>::getName(targetFormat)<<std::endl;

  // domyœlnie pauzujemy
  _status = PAUSED;

  // uda³o siê!
  return true;
}

void VideoImageStream::play()
{
  VM_FUNCTION_PROLOG;
  if ( !isRunning() ) {
    // uruchamiamy watek
    _status = PLAYING;
    start();
  } else {
    _status = PLAYING;
  }
}

void VideoImageStream::pause()
{
  VM_FUNCTION_PROLOG;
  _status = PAUSED;
}

void VideoImageStream::rewind()
{
  VM_FUNCTION_PROLOG;
  // trzeba synchronizowaæ
  ScopedLock lock(streamStateMutex);
  setStreamTime(0.0);
  publishFrame();
}

void VideoImageStream::seek(double time)
{
  VM_FUNCTION_PROLOG;
  ScopedLock lock(streamStateMutex);
  setStreamTime(time);
  publishFrame();
}

void VideoImageStream::quit(bool waitForThreadToExit)
{
  VM_FUNCTION_PROLOG;
  ScopedLock lock(streamStateMutex);
  if ( isRunning() ) {
    // zmieniamy stan...
    _status = PAUSED;
    if ( waitForThreadToExit ) {
      join();
    }
  }
  // kasujemy
  innerStream = NULL;
  currentPicture.free();
}

double VideoImageStream::getCreationTime() const
{
  OSG_NOTICE<<"Not implemented."<<std::endl;
  return 0.0;
}

double VideoImageStream::getLength() const
{
  return getStream()->getDuration();
}

double VideoImageStream::getReferenceTime () const
{
  return getStream()->getTime();
}

double VideoImageStream::getFrameRate() const
{
  return getStream()->getFramerate();
}

bool VideoImageStream::isImageTranslucent() const
{
  return false;
}

void VideoImageStream::applyLoopingMode()
{
  ImageStream::applyLoopingMode();
}

void VideoImageStream::setTimeMultiplier( double timescale )
{
  timeScale = timescale;
}

double VideoImageStream::getTimeMultiplier() const
{
  return timeScale;
}

void VideoImageStream::update( osg::NodeVisitor* nv )
{  
}

bool VideoImageStream::requiresUpdateCall() const
{
  return false;
}

void VideoImageStream::setMaxWidth( int maxWidth )
{
  VM_FUNCTION_PROLOG;
  this->maxWidth = maxWidth;

  int width = 0;
  int height = 0;
  int streamWidth = getStream()->getWidth();
  int streamHeight = getStream()->getHeight();

  // jaka szerokoœæ zostanie zastosowana?
  if ( maxWidth <=0 || maxWidth >= streamWidth ) {
    width = streamWidth;
    height = streamHeight;
  } else {
    int widthDiv = static_cast<int>((static_cast<double>(streamWidth)/maxWidth + 0.334));
    width = streamWidth / widthDiv;
    height = streamHeight / widthDiv;
    //if ( targetFormat == vm::PixelFormatYV12 ) {
      width = UTILS_ALIGN(width, 2);
      height = UTILS_ALIGN(height, 2);
    //}
  }

  // czy realokacja ramki?
  if ( currentPicture.width != width ) {
    OSG_NOTICE<<getStream()->getSource()<<":Changing frame size from "<<currentPicture.width<<" to "<<width<<std::endl;
    ScopedLock lock(streamStateMutex);
    currentPicture.free();
    currentPicture = vm::Picture::create(width, height, targetFormat);
    reloadImage(currentPicture);
  }
}

void VideoImageStream::setStreamTime( double time )
{
  VM_FUNCTION_PROLOG;
  getStream()->setTime(time);
  timer.setStartTick();
}

void VideoImageStream::publishFrame()
{
  VM_FUNCTION_PROLOG;
  // poprzedni timestamp
  if ( prevTimestamp != getStream()->getFrameTimestamp() ) {
    prevTimestamp = getStream()->getFrameTimestamp();
    reloadImage(currentPicture);
  }
}

void VideoImageStream::reloadImage(vm::Picture & picture)
{
  VM_FUNCTION_PROLOG;
  getStream()->getFrame(picture);
  switch (targetFormat) {

    case vm::PixelFormatYV12:
      setImage(picture.width, picture.dataHeight,
        1, GL_LUMINANCE, GL_LUMINANCE, GL_UNSIGNED_BYTE,
        picture.data, NO_DELETE);
      break;
    
    case vm::PixelFormatRGB24:
      setImage(picture.width, picture.height,
        1, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE,
        picture.data, NO_DELETE);
      break;

    case vm::PixelFormatBGRA:
      setImage(picture.width, picture.height,
        1, GL_RGBA, GL_BGRA, GL_UNSIGNED_BYTE,
        picture.data, NO_DELETE);
      break;

    default:
      OSG_FATAL<<"Unknown image format."<<std::endl;
      setImage(picture.width, picture.dataHeight,
        1, GL_LUMINANCE, GL_LUMINANCE, GL_UNSIGNED_BYTE,
        picture.data, NO_DELETE);
      break;

  }
}

void VideoImageStream::setTargetFormat( vm::PixelFormat targetFormat )
{
    if ( targetFormat != this->targetFormat ) {

        if ( targetFormat == vm::PixelFormatYV12 ) {
            // poniewa¿ wymagamy konwersji w shaderach, ustawiamy aspect ratio
            setPixelAspectRatio( static_cast<float>(getStream()->getAspectRatio()) * 1.5f );
        } else {
            setPixelAspectRatio( static_cast<float>(getStream()->getAspectRatio()));
        }

        this->targetFormat = targetFormat;
        int width = currentPicture.width;
        int height = currentPicture.height;
        currentPicture.free();
        currentPicture.format = targetFormat;
        currentPicture = vm::Picture::create(width, height, targetFormat);
        reloadImage(currentPicture);
    }
}


} // namespace vmOSGPlugin
