/********************************************************************
	created:	2011/05/12
	created:	12:5:2011   9:12
	filename: 	IInputProcessItem.h
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__IINPUTPROCESSITEM_H__
#define HEADER_GUARD_CORE__IINPUTPROCESSITEM_H__

#include <core/IWorkflowItemBase.h>
#include <core/IInputDescription.h>

namespace core {

	class IObjectSource;
//! Interfejs dla elementów terminalnych dla danych - nie mających wyjścia jedynie wejscie.
//! Zawiera opis wejśc i podstawowe informacje i właściwości elementów logicznych aplikacji operujących na danych
class IInputProcessItem : public IWorkflowItemBase, public IInputDescription
{
public:
    //! Metoda wywoływana w momencie dostarczenia nowych danych do przetwarzania
    //! \param source Źródło danych. NIE MA GWARANCJI ŻE MAMY JAKIEKOLWIEK DANE, należy to zawsze sprawdzać przy każdym wywołaniu
    virtual void setUp(IObjectSource* source) = 0;
};

}

#endif
