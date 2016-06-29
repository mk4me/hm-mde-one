/********************************************************************
    created:  2012/12/17
    created:  17:12:2012   8:54
    filename: IDFPin.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IDFPIN_H__
#define HEADER_GUARD___IDFPIN_H__

#include <dflib/IPin.h>
#include <dflib/IDFFeatures.h>

////////////////////////////////////////////////////////////////////////////////
namespace df {
////////////////////////////////////////////////////////////////////////////////

//! Zbi�r interfejs�w na potrzeby przetwarzania danych - przekazywania danych
//! mi�dzy w�z�ami przez piny. Aby stworzy� pin z danymi �wiadomy przetwarzania danych
//! i ich przekazywania nale�y rozszerzy� implementacj� podstaowych interfejs�w pin�w
//! o odpowiednie interfejsy zamieszczone poni�ej

//! Interfejs pinu wyj�ciowego �wiadomego danych
class IDFOutput : public IDFResetable
{

};

//! Interfejs pinu wej�ciowego �wiadomego danych
class IDFInput : public IDFResetable
{
public:
	//! \param pin Wyj�ciowy pin z kt�rego kopiujemy dane
	virtual void copyData(const IDFOutput * pin) = 0;
};


}

#endif	//	HEADER_GUARD___IDFPIN_H__
