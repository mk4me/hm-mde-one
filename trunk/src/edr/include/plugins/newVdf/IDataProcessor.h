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
	};
	typedef core::shared_ptr<IDataProcessor> IDataProcessorPtr;
	typedef core::shared_ptr<const IDataProcessor> IDataProcessorConstPtr;

} 

#endif
