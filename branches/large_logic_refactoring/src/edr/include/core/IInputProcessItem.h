/********************************************************************
	created:	2011/05/12
	created:	12:5:2011   9:12
	filename: 	IInputProcessItem.h
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__IINPUTPROCESSITEM_H__
#define HEADER_GUARD_CORE__IINPUTPROCESSITEM_H__

#include <core/IIdentifiable.h>
#include <core/IInputDescription.h>

namespace plugin {

	class IObjectSource;
//! Interfejs dla elementów terminalnych dla danych - nie mających wyjścia jedynie wejscie.
//! Zawiera opis wejśc i podstawowe informacje i właściwości elementów logicznych aplikacji operujących na danych
class IInputProcessItem : public IIdentifiable, public IInputDescription
{
public:
    //! Metoda wywoływana w momencie dostarczenia nowych danych do przetwarzania
    //! \param source Źródło danych. NIE MA GWARANCJI ŻE MAMY JAKIEKOLWIEK DANE, należy to zawsze sprawdzać przy każdym wywołaniu
    virtual void consume(IObjectSource* source) = 0;
};

}

#endif
