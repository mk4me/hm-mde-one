#ifndef __AVINOUT_FFMPEGEXCEPTION_H__
#define __AVINOUT_FFMPEGEXCEPTION_H__

////////////////////////////////////////////////////////////////////////////////
namespace vm {
////////////////////////////////////////////////////////////////////////////////

//! Wyj�tek u�ywany przez bibliotek�.
class FFmpegError : public VideoError
{
public:
  FFmpegError(const std::string& msg, int id = VideoError::UnknownID) : VideoError(msg, id)
  {}

  FFmpegError(const FFmpegError& error) : VideoError(error)
  {}

  //! \return Klon obiektu.
  AVOVERRIDE VideoError * clone() const
  {
    return new FFmpegError(*this);
  }
};

////////////////////////////////////////////////////////////////////////////////
} // namespace vm
////////////////////////////////////////////////////////////////////////////////

#endif  // __AVINOUT_FFMPEGEXCEPTION_H__