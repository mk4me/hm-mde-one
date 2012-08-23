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
//! Interfejs dla elementÛw terminalnych dla danych - nie majπcych wyjúcia jedynie wejscie.
//! Zawiera opis wejúc i podstawowe informacje i w≥aúciwoúci elementÛw logicznych aplikacji operujπcych na danych
class IInputProcessItem : public IWorkflowItemBase, public IInputDescription
{
public:
    //! Metoda wywo≥ywana w momencie dostarczenia nowych danych do przetwarzania
    //! \param source èrÛd≥o danych. NIE MA GWARANCJI ØE MAMY JAKIEKOLWIEK DANE, naleøy to zawsze sprawdzaÊ przy kaødym wywo≥aniu
    virtual void setUp(IObjectSource* source) = 0;
};

}

#endif