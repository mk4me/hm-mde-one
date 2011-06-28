#ifndef HEADER_GUARD__CONNECTION_H__
#define HEADER_GUARD__CONNECTION_H__

#include <dfmlib/DFLMTypes.h>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////


//! Klasa opisuj¹ca po³¹czenie dwóch pinów. Mo¿na z niej pobraæ piny w uporz¹dkowanej kolejnoœci - pin wyjœciowy (Ÿród³o) - pin wejœciowy (odbiorca).
class Connection
{

public:

    //! Klasa zaprzyjaŸniona - model ma pe³n¹ kontrolê nad po³¹czeniami - sam je tworzy i nimi zarz¹dza, u¿ytkownik tylko z nich korzysta
	friend class Model;

public:

    //! Konstruktor chroniony, pozwala dziedziczyæ ale nie pozwala klientowi samemu tworzyæ po³¹czenia i ich modyfikowaæ
    //! \param src Wêze³ Ÿród³owy
    //! \param dest Wêze³ docelowy
    Connection(const PinPtr & src, const PinPtr & dest);

    //! Wirtualny destruktor
	virtual ~Connection(void);

    //! \return Weze³ Ÿród³owy
	PinPtr getSrc() const;

    //! \return Weze³ docelowy
	PinPtr getDest() const;

    //! \return drugi wêze³ bior¹cy udzia³ w po³¹czeniu lub wyj¹tek jesli wêze³ nie bierze udzia³u w tym po³¹czeniu
	PinPtr getOther(const CWPinPtr & pin) const;

    //! \return Nazwa po³aczenia
	std::string getConnectionName() const;

private:
    //! \param src Wêze³ Ÿród³owy(wyjœciowy)
	void setSrc(const PinPtr & src);

    //! \param dest Wêze³ docelowy(wejœciowy)
	void setDest(const PinPtr & dest);

private:
    //! Wêze³ Ÿród³owy po³¹czenia
	WPinPtr srcPin;

    //! Wêze³ docelowy po³aczenia
	WPinPtr destPin;
};

}

#endif