/********************************************************************
	created:	2013/01/07
	created:	7:1:2013   20:50
	filename: 	IDataSink.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD___IDATASINK_H__
#define HEADER_GUARD___IDATASINK_H__

#include <plugins/newVdf/IInputDescription.h>
#include <plugins/newVdf/IWorkflowItemBase.h>
//! Forward declaration
class QIcon;

namespace vdf {
    
//! Interfejs do implemntacji ujść danych
class IDataSink : public IInputDescription, public IWorkflowItemBase
{
public:
    //! \return Dialog konfigurujący opis wyjścia źródła danych
    //virtual QDialog * getOutputConfigurationDialog() = 0;
    //! \return Ikona źródła
    //virtual QIcon* getIcon() const = 0;
};

typedef core::shared_ptr<IDataSink> IDataSinkPtr;
typedef core::shared_ptr<const IDataSink> IDataSinkConstPtr;

}

#endif 
