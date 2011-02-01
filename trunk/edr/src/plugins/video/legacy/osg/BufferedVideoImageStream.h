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
 *	Strumieñ obrazków z z buforowaniem.
 */
class BufferedVideoImageStream : public VideoImageStream
{
private:
  //! W¹tek buforuj¹cy.
  class Bufferer : public OpenThreads::Thread
  {
  private:
    //! Strumieñ buforowany.
    osg::observer_ptr<BufferedVideoImageStream> stream;
  public:
    //! Konstruktor.
    Bufferer(BufferedVideoImageStream * stream);
    //! Funkcja buforuj¹ca.
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
  /** Konstruktor zeruj¹cy */
  BufferedVideoImageStream();
  /** Konstruktor kopiuj¹cy */
  BufferedVideoImageStream(const BufferedVideoImageStream & image, const osg::CopyOp & copyop = osg::CopyOp::SHALLOW_COPY);
  /** Domyœlne implementacje */
  META_Object(osgPlugin, BufferedVideoImageStream);

  /** Otwiera strumieñ */
  virtual bool open(const std::string & filename);

protected:
  /** Destruktor */
  virtual ~BufferedVideoImageStream();

public:
  /** Czas w strumieniu */
  virtual double getReferenceTime() const;


  /** Ustawia bie¿¹c¹ ramkê (jeœli jest nowsza ni¿ poprzednia)*/
  virtual void publishFrame();
  /** Ustawia bie¿¹cy czas */
  virtual void setStreamTime( double time );


};


////////////////////////////////////////////////////////////////////////////////
} // namespace osgPlugin
} // namespace video
////////////////////////////////////////////////////////////////////////////////



#endif  // __HEADER_GUARD__BUFFEREDVIDEOIMAGESTREAM_H__