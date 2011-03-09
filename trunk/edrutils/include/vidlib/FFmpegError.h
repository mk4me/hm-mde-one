#ifndef HEADER_GUARD_VIDLIB__FFMPEGERROR_H__
#define HEADER_GUARD_VIDLIB__FFMPEGERROR_H__

#include <vidlib/VideoError.h>
#include <vidlib/Export.h>

////////////////////////////////////////////////////////////////////////////////
namespace vidlib {
////////////////////////////////////////////////////////////////////////////////

//! Wyj�tek u�ywany przez bibliotek�.
class VIDLIB_EXPORT FFmpegError : public VideoError
{
public:
    enum {
        //! B�ad posiada nieznane ID.
        UnknownID = -1
    };

private:
    //! Wewn�trzne ID b��du.
    const int id;

public:
    FFmpegError(const char* msg, int id = UnknownID);
    FFmpegError(const std::string& msg, int id = UnknownID);
    FFmpegError(const FFmpegError& error);

    //! \return Wewn�trzne ID b��du.
    inline int getID()
    {
        return id;
    }

    //! \return Klon obiektu.
    virtual VideoError * clone() const;

protected:
    //! Specjalnie niewirtualna!
    void setMessage(const char* msg, size_t length);
};

////////////////////////////////////////////////////////////////////////////////
} // namespace vidlib
////////////////////////////////////////////////////////////////////////////////

#endif  // HEADER_GUARD_VIDLIB__FFMPEGERROR_H__