/********************************************************************
	created:	2011/05/12
	created:	12:5:2011   9:12
	filename: 	IInputOutputProcessItem.h
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__IINPUTOUTPUTPROCESSITEM_H__
#define HEADER_GUARD_CORE__IINPUTOUTPUTPROCESSITEM_H__

#include <core/IWorkflowItemBase.h>
#include <core/IInputDescription.h>
#include <core/IOutputDescription.h>
//#include <core/IObjectSource.h>
#include <core/IObjectOutput.h>

namespace core {

	class  IObjestSource;
//! Interfejs dla element�w przetwarzaj�cych.
//! Zawiera opis wej�c i wyj��, podstawowe informacje i w�a�ciwo�ci element�w logicznych aplikacji operuj�cych na danych
class IInputOutputProcessItem : public IWorkflowItemBase, public IInputDescription, public IOutputDescription
{
public:
    //! Przetwarza informacje.
    //! \param input Obiekt z danymi wejsciowymi, NIE MA GWARANCJI �E DANE S� DOST�PNE - nale�y to sprawdza� przy ka�dym wywo�aniu
    //! \param output Miejsce zapisu danych wyjsciowych, przekazywanych dalej, MAMY ZAGWARANTOWANE �E DANE MO�NA ZAWSZE zapisywa� do wyj�cia (kwestia ich kompatybilno�ci z tym co zadeklarowano)
    virtual void process(IObjectSource* input, IObjectOutput* output) = 0;
};

}

#endif