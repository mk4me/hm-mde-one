/********************************************************************
    created:  2011/04/14
    created:  14:4:2011   17:29
    filename: IDataProcessor.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__IDATAPROCESSOR_H__
#define HEADER_GUARD_CORE__IDATAPROCESSOR_H__

#include <core/SmartPtr.h>
#include <core/IInputOutputProcessItem.h>

namespace plugin 
{
    //! Definicja typu elementu przetwarzającego
    typedef core::IInputOutputProcessItem IDataProcessor;

    typedef core::shared_ptr<IDataProcessor> IDataProcessorPtr;
    typedef core::shared_ptr<const IDataProcessor> IDataProcessorConstPtr;
} // namespace core


#endif  // HEADER_GUARD_CORE__IDATAPROCESSOR_H__
