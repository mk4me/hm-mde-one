#include "../PCH.h"
#include <vidlib/osg/BufferedVideoImageStream.h>

////////////////////////////////////////////////////////////////////////////////
namespace vidlib {
////////////////////////////////////////////////////////////////////////////////

BufferedVideoImageStream::BufferedVideoImageStream()
: VideoImageStream()
{
}

BufferedVideoImageStream::BufferedVideoImageStream( const BufferedVideoImageStream & image, const osg::CopyOp & copyop /*= osg::CopyOp::SHALLOW_COPY*/ )
: VideoImageStream(image, copyop)
{
  throw std::runtime_error("Not supported");
}


BufferedVideoImageStream::~BufferedVideoImageStream()
{

}

bool BufferedVideoImageStream::open( const std::string & /*filename*/ )
{
  // czy uda�o si� otworzy� strumie�?
//   if ( !VideoImageStream::open(filename) ) {
//     return false;
//   }
  // inicjalizacja bufor�w
  // mie�cimy indeksy sekund
  //thumbnailsMap.resize( static_cast<size_t>((*innerStream)->getDuration()) + 1, -1);
  


  return true;
}




double BufferedVideoImageStream::getReferenceTime() const
{
  return wantedTime;
}


void BufferedVideoImageStream::publishFrame()
{

}

void BufferedVideoImageStream::setStreamTime( double /*time*/ )
{

}
////////////////////////////////////////////////////////////////////////////////
} // namespace vidlib
////////////////////////////////////////////////////////////////////////////////