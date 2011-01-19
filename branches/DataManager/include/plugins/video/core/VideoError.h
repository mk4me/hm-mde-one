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

//! Wyj�tek u�ywany przez bibliotek�.
class VideoError : public std::runtime_error
{
public:
  enum {
    //! B�ad posiada nieznane ID.
    UnknownID = 0xFFFFFFFF
  };

private:
  //! Wewn�trzne ID b��du.
  int internalID;

public:
  
  //! \param msg Wiadomo�� o b��dzie.
  //! \return id ID b��du.
  VideoError(const std::string& msg, int id = UnknownID) : runtime_error(msg), internalID(id)
  {}

  //! \param msg Wiadomo�� o b��dzie.
  //! \return id ID b��du.
  VideoError(const VideoError& error) : runtime_error(error.what()), internalID(error.internalID)
  {}

  //! \return Wewn�trzne ID b��du.
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
  // wirtualna metoda t�umacz�ca ID na jaki� komunikat
};

////////////////////////////////////////////////////////////////////////////////
} // namespace video
////////////////////////////////////////////////////////////////////////////////

#endif  // __AVINOUT_VIDEOERROR_H__