/********************************************************************
    created:  2011/04/14
    created:  14:4:2011   17:29
    filename: IDataProcessor.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__IDATAPROCESSOR_H__
#define HEADER_GUARD_CORE__IDATAPROCESSOR_H__

#include <corelib/SmartPtr.h>
#include <plugins/newVdf/IInputDescription.h>
#include <plugins/newVdf/IOutputDescription.h>
#include <plugins/newVdf/IWorkflowItemBase.h>
//! Forward declaration
class QIcon;

namespace vdf 
{
	//! Interfejs do implemntacji ujść danych
	class IDataProcessor : public IOutputDescription, public IInputDescription, public IWorkflowItemBase
	{
	public:
		////! \return Dialog konfigurujący opis wyjścia źródła danych
		//virtual QDialog * getOutputConfigurationDialog() = 0;
		////! \return Czy źródło może jeszcze generować kolejne dane
		//virtual bool empty() const = 0;
		////! \return Ikona źródła
		//virtual QIcon* getIcon() const = 0;
	};

	typedef core::shared_ptr<IDataProcessor> IDataProcessorPtr;
	typedef core::shared_ptr<const IDataProcessor> IDataProcessorConstPtr;

} // namespace core


#endif  // HEADER_GUARD_CORE__IDATAPROCESSOR_H__
