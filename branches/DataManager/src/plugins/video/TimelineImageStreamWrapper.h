/********************************************************************
	created:  2010/05/19
	created:  19:5:2010   19:00
	filename: TimelineImageStreamWrapper.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__TIMELINEOSGIMAGESTREAM_H__
#define __HEADER_GUARD__TIMELINEOSGIMAGESTREAM_H__

#include <plugins/timeline/Stream.h>

/**
 * 
 */
class TimelineOSGImageStream : public timeline::Stream
{
private:
  //! Strumieñ wewnêtrzny.
  osg::ref_ptr<osg::ImageStream> stream;

public:
  //! \param stream Strumieñ wewnêtrzny.
  TimelineOSGImageStream(osg::ref_ptr<osg::ImageStream> stream) : stream(stream)
  {}
  //! \see Stream::setTime
  virtual void setTime(double time)
  {
    stream->seek(time);
  }
  //! \see Stream::getTime
  virtual double getTime() const
  {
    return stream->getReferenceTime();
  }
  //! \see Stream::getLength
  virtual double getLength() const
  {
    return stream->getLength();
  }
};

////////////////////////////////////////////////////////////////////////////////
namespace timeline {
////////////////////////////////////////////////////////////////////////////////
template<> inline Stream* Stream::encapsulate(osg::ImageStream * stream)
{
  return new TimelineOSGImageStream(stream);
}
////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////


#endif  // __HEADER_GUARD__TIMELINEOSGIMAGESTREAM_H__