#ifndef HEADER_GUARD_VIDLIB__FFMPEGERROR_H__
#define HEADER_GUARD_VIDLIB__FFMPEGERROR_H__

#include <vidlib/VideoError.h>
#include <vidlib/Export.h>

////////////////////////////////////////////////////////////////////////////////
namespace vidlib {
////////////////////////////////////////////////////////////////////////////////

//! Wyj¹tek u¿ywany przez bibliotekê.
class VIDLIB_EXPORT FFmpegError : public VideoError
{
public:
    enum {
        //! B³ad posiada nieznane ID.
        UnknownID = -1
    };

private:
    //! Wewnêtrzne ID b³êdu.
    const int id;

public:
    FFmpegError(const char* msg, int id = UnknownID);
    FFmpegError(const std::string& msg, int id = UnknownID);
    FFmpegError(const FFmpegError& error);

    //! \return Wewnêtrzne ID b³êdu.
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