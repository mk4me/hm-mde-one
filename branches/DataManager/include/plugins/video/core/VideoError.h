/********************************************************************
	created:  2010/02/11
	created:  11:2:2010   13:14
	filename: VideoError.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __AVINOUT_VIDEOERROR_H__
#define __AVINOUT_VIDEOERROR_H__

#include <stdexcept>

////////////////////////////////////////////////////////////////////////////////
namespace video {
////////////////////////////////////////////////////////////////////////////////

//! Wyj¹tek u¿ywany przez bibliotekê.
class VideoError : public std::runtime_error
{
public:
  enum {
    //! B³ad posiada nieznane ID.
    UnknownID = 0xFFFFFFFF
  };

private:
  //! Wewnêtrzne ID b³êdu.
  int internalID;

public:
  
  //! \param msg Wiadomoœæ o b³êdzie.
  //! \return id ID b³êdu.
  VideoError(const std::string& msg, int id = UnknownID) : runtime_error(msg), internalID(id)
  {}

  //! \param msg Wiadomoœæ o b³êdzie.
  //! \return id ID b³êdu.
  VideoError(const VideoError& error) : runtime_error(error.what()), internalID(error.internalID)
  {}

  //! \return Wewnêtrzne ID b³êdu.
  int getID()
  {
    return internalID;
  }

  //! \return Klon obiektu.
  virtual VideoError * clone() const
  {
    return new VideoError(*this);
  }

  // TODO
  // wirtualna metoda t³umacz¹ca ID na jakiœ komunikat
};

////////////////////////////////////////////////////////////////////////////////
} // namespace video
////////////////////////////////////////////////////////////////////////////////

#endif  // __AVINOUT_VIDEOERROR_H__