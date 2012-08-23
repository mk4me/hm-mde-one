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
//! Interfejs dla element�w terminalnych dla danych - nie maj�cych wyj�cia jedynie wejscie.
//! Zawiera opis wej�c i podstawowe informacje i w�a�ciwo�ci element�w logicznych aplikacji operuj�cych na danych
class IInputProcessItem : public IWorkflowItemBase, public IInputDescription
{
public:
    //! Metoda wywo�ywana w momencie dostarczenia nowych danych do przetwarzania
    //! \param source �r�d�o danych. NIE MA GWARANCJI �E MAMY JAKIEKOLWIEK DANE, nale�y to zawsze sprawdza� przy ka�dym wywo�aniu
    virtual void setUp(IObjectSource* source) = 0;
};

}

#endif