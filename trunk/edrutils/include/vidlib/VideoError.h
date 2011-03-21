/********************************************************************
	created:  2010/02/11
	created:  11:2:2010   13:14
	filename: VideoError.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_VIDLIB__VIDEOERROR_H__
#define HEADER_GUARD_VIDLIB__VIDEOERROR_H__

#include <stdexcept>
#include <vidlib/Export.h>

namespace vidlib {

//! Wyj�tek u�ywany przez bibliotek�.
class VIDLIB_EXPORT VideoError : public std::exception
{
private:
    //! Wg. standradu std::exception NIE ma konstruktora przyjmuj�cego
    //! const char*, z tego powodu trzymamy r�czn� kopi�.
    char* msgCopy;

protected:
    //! Konstruktor zeruj�cy.
    VideoError();

public:
    //! \param msg Wiadomo�� o b��dzie.
    //! \return id ID b��du.
    VideoError(const char* msg);
    //! \param msg Wiadomo�� o b��dzie.
    //! \return id ID b��du.
    VideoError(const std::string& msg);
    //! \param error Wyj�tek �r�d�owy.
    VideoError(const VideoError& error);

    //! Polimorficzny destruktor.
    virtual ~VideoError();

    //! \return Klon obiektu.
    virtual VideoError * clone() const;
    //! \return Komunikat b��du.
    virtual const char* what() const throw();

protected:
    //! Kopiuje wiadomo�� do wewn�trznego bufora. Specjalnie niewirtualna.
    void setMessage( const char* msg, size_t length );
};

} // namespace vidlib

#endif  // HEADER_GUARD_VIDLIB__VIDEOERROR_H__