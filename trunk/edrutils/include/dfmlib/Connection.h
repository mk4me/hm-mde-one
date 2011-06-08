#ifndef HEADER_GUARD__CONNECTION_H__
#define HEADER_GUARD__CONNECTION_H__

#include <dfmlib/DFLMTypes.h>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////


//! Klasa opisuj�ca po��czenie dw�ch pin�w. Mo�na z niej pobra� piny w uporz�dkowanej kolejno�ci - pin wyj�ciowy (�r�d�o) - pin wej�ciowy (odbiorca).
class Connection
{

public:

    //! Klasa zaprzyja�niona - model ma pe�n� kontrol� nad po��czeniami - sam je tworzy i nimi zarz�dza, u�ytkownik tylko z nich korzysta
	friend class Model;

public:

    //! Konstruktor chroniony, pozwala dziedziczy� ale nie pozwala klientowi samemu tworzy� po��czenia i ich modyfikowa�
    //! \param src W�ze� �r�d�owy
    //! \param dest W�ze� docelowy
    Connection(const PinPtr & src, const PinPtr & dest);

    //! Wirtualny destruktor
	virtual ~Connection(void);

    //! \return Weze� �r�d�owy
	PinPtr getSrc() const;

    //! \return Weze� docelowy
	PinPtr getDest() const;

    //! \return drugi w�ze� bior�cy udzia� w po��czeniu lub wyj�tek jesli w�ze� nie bierze udzia�u w tym po��czeniu
	PinPtr getOther(const CWPinPtr & pin) const;

    //! \return Nazwa po�aczenia
	std::string getConnectionName() const;

private:
    //! \param src W�ze� �r�d�owy(wyj�ciowy)
	void setSrc(const PinPtr & src);

    //! \param dest W�ze� docelowy(wej�ciowy)
	void setDest(const PinPtr & dest);

private:
    //! W�ze� �r�d�owy po��czenia
	WPinPtr srcPin;

    //! W�ze� docelowy po�aczenia
	WPinPtr destPin;
};

}

#endif