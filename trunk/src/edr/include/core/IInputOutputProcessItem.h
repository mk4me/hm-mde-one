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

namespace core {

	class IObjectSource;
	class IObjectOutput;

//! Interfejs dla elementów przetwarzających.
//! Zawiera opis wejśc i wyjść, podstawowe informacje i właściwości elementów logicznych aplikacji operujących na danych
class IInputOutputProcessItem : public IWorkflowItemBase, public IInputDescription, public IOutputDescription
{
public:

	virtual ~IInputOutputProcessItem() {}

    //! Przetwarza informacje.
    //! \param input Obiekt z danymi wejsciowymi, NIE MA GWARANCJI ŻE DANE SĄ DOSTĘPNE - należy to sprawdzać przy każdym wywołaniu
    //! \param output Miejsce zapisu danych wyjsciowych, przekazywanych dalej, MAMY ZAGWARANTOWANE ŻE DANE MOŻNA ZAWSZE zapisywać do wyjścia (kwestia ich kompatybilności z tym co zadeklarowano)
    virtual void process(IObjectSource* input, IObjectOutput* output) = 0;
};

}

#endif
