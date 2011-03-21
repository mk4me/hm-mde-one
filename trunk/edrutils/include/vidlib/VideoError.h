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

//! Wyj¹tek u¿ywany przez bibliotekê.
class VIDLIB_EXPORT VideoError : public std::exception
{
private:
    //! Wg. standradu std::exception NIE ma konstruktora przyjmuj¹cego
    //! const char*, z tego powodu trzymamy rêczn¹ kopiê.
    char* msgCopy;

protected:
    //! Konstruktor zeruj¹cy.
    VideoError();

public:
    //! \param msg Wiadomoœæ o b³êdzie.
    //! \return id ID b³êdu.
    VideoError(const char* msg);
    //! \param msg Wiadomoœæ o b³êdzie.
    //! \return id ID b³êdu.
    VideoError(const std::string& msg);
    //! \param error Wyj¹tek Ÿród³owy.
    VideoError(const VideoError& error);

    //! Polimorficzny destruktor.
    virtual ~VideoError();

    //! \return Klon obiektu.
    virtual VideoError * clone() const;
    //! \return Komunikat b³êdu.
    virtual const char* what() const throw();

protected:
    //! Kopiuje wiadomoœæ do wewnêtrznego bufora. Specjalnie niewirtualna.
    void setMessage( const char* msg, size_t length );
};

} // namespace vidlib

#endif  // HEADER_GUARD_VIDLIB__VIDEOERROR_H__