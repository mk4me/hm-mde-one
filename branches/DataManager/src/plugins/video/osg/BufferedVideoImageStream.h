/********************************************************************
	created:  2010/07/22
	created:  22:7:2010   10:25
	filename: BufferedImageStream.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__BUFFEREDVIDEOIMAGESTREAM_H__
#define __HEADER_GUARD__BUFFEREDVIDEOIMAGESTREAM_H__

#include <plugins/video/osg/VideoImageStream.h>
#include <osg/observer_ptr>

////////////////////////////////////////////////////////////////////////////////
namespace video {
namespace osgPlugin {
////////////////////////////////////////////////////////////////////////////////

/**
 *	Strumie� obrazk�w z z buforowaniem.
 */
class BufferedVideoImageStream : public VideoImageStream
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
  META_Object(osgPlugin, BufferedVideoImageStream);

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


////////////////////////////////////////////////////////////////////////////////
} // namespace osgPlugin
} // namespace video
////////////////////////////////////////////////////////////////////////////////



#endif  // __HEADER_GUARD__BUFFEREDVIDEOIMAGESTREAM_H__