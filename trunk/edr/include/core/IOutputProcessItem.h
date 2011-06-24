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
#include <core/IObjectOutput.h>

//! Forward declaration
class QDialog;

namespace core {

//! Interfejs element�w dostarczaj�cych danych do aplikacji (workflow).
//! Zapewnia opis danych wyj�ciowych i podstawowe informacje i w�a�ciwo�ci element�w logicznych aplikacji operuj�cych na danych
class IOutputProcessItem : public IOutputDescription, public IWorkflowItemBase
{
public:
    //! Metoda dostarczaj�ca kolejn� porcj� danych podczas przetwarzania
    virtual void generate(IObjectOutput* output) = 0;
};

}

#endif