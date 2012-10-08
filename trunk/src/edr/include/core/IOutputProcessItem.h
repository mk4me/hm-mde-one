/********************************************************************
	created:	2011/05/12
	created:	12:5:2011   9:13
	filename: 	IOutputProcessItem.h
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__IOUTPUTPROCESSITEM_H__
#define HEADER_GUARD_CORE__IOUTPUTPROCESSITEM_H__

#include <core/IOutputDescription.h>
#include <core/IWorkflowItemBase.h>

//! Forward declaration
class QDialog;

namespace core {

	class IObjectOutput;

//! Interfejs elementów dostarczających danych do aplikacji (workflow).
//! Zapewnia opis danych wyjściowych i podstawowe informacje i właściwości elementów logicznych aplikacji operujących na danych
class IOutputProcessItem : public IOutputDescription, public IWorkflowItemBase
{
public:
    //! Metoda dostarczająca kolejną porcję danych podczas przetwarzania
    virtual void generate(IObjectOutput* output) = 0;
};

}

#endif
