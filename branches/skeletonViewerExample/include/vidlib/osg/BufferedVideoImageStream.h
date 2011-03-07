/********************************************************************
	created:  2010/07/22
	created:  22:7:2010   10:25
	filename: BufferedImageStream.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_VIDLIB__BUFFEREDVIDEOIMAGESTREAM_H__
#define __HEADER_GUARD_VIDLIB__BUFFEREDVIDEOIMAGESTREAM_H__

#include <utils/Utils.h>
UTILS_PUSH_WARNINGS
#include <osg/observer_ptr>
UTILS_POP_WARNINGS
#include <vidlib/osg/VideoImageStream.h>
#include <vidlib/VideoBuffer.h>
#include <vidlib/Export.h>

////////////////////////////////////////////////////////////////////////////////
namespace vidlib {
////////////////////////////////////////////////////////////////////////////////
UTILS_PUSH_WARNINGS
UTILS_DISABLE_DLL_INTERFACE_WARNING

/**
 *	Strumie� obrazk�w z z buforowaniem.
 */
class VIDLIB_EXPORT BufferedVideoImageStream : public VideoImageStream
{
private:
  //! W�tek buforuj�cy.
  class Bufferer : public OpenThreads::Thread
  {
  private:
    //! Strumie� buforowany.
    osg::observer_ptr<BufferedVideoImageStream> stream;
  public:
    //! Konstruktor.
    Bufferer(BufferedVideoImageStream * stream);
    //! Funkcja buforuj�ca.
    virtual void run();
  };
  //! Wrapper bufora ramek.
  typedef utils::PtrWrapper<VideoBuffer, utils::PtrPolicyOSG> FrameBuffer;

private:
  //! Bufor ramek.
  osg::ref_ptr<FrameBuffer> frameBuffer;
  //! Czas strumienia.
  double wantedTime;

public:
  /** Konstruktor zeruj�cy */
  BufferedVideoImageStream();
  /** Konstruktor kopiuj�cy */
  BufferedVideoImageStream(const BufferedVideoImageStream & image, const osg::CopyOp & copyop = osg::CopyOp::SHALLOW_COPY);
  /** Domy�lne implementacje */
  META_Object(vidlib, BufferedVideoImageStream);

  /** Otwiera strumie� */
  virtual bool open(const std::string & filename);

protected:
  /** Destruktor */
  virtual ~BufferedVideoImageStream();

public:
  /** Czas w strumieniu */
  virtual double getReferenceTime() const;


  /** Ustawia bie��c� ramk� (je�li jest nowsza ni� poprzednia)*/
  virtual void publishFrame();
  /** Ustawia bie��cy czas */
  virtual void setStreamTime( double time );


};

UTILS_POP_WARNINGS
////////////////////////////////////////////////////////////////////////////////
} // namespace vidlib
////////////////////////////////////////////////////////////////////////////////

#endif  // __HEADER_GUARD_VIDLIB__BUFFEREDVIDEOIMAGESTREAM_H__