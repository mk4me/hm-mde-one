#ifndef __HEADER_GUARD_VIDLIB__FFMPEGERROR_H__
#define __HEADER_GUARD_VIDLIB__FFMPEGERROR_H__

#include <vidlib/VideoError.h>
#include <vidlib/Export.h>

////////////////////////////////////////////////////////////////////////////////
namespace vidlib {
////////////////////////////////////////////////////////////////////////////////

//! Wyj¹tek u¿ywany przez bibliotekê.
class FFmpegError : public VideoError
{
public:
  FFmpegError(const std::string& msg, int id = VideoError::UnknownID) : VideoError(msg, id)
  {}

  FFmpegError(const FFmpegError& error) : VideoError(error)
  {}

  //! \return Klon obiektu.
  virtual VideoError * clone() const
  {
    return new FFmpegError(*this);
  }
};

////////////////////////////////////////////////////////////////////////////////
} // namespace vidlib
////////////////////////////////////////////////////////////////////////////////

#endif  // __HEADER_GUARD_VIDLIB__FFMPEGERROR_H__