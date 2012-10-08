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

namespace core 
{
    //! Definicja typu elementu przetwarzającego
    typedef IInputOutputProcessItem IDataProcessor;

    typedef shared_ptr<IDataProcessor> IDataProcessorPtr;
    typedef shared_ptr<const IDataProcessor> IDataProcessorConstPtr;
} // namespace core


#endif  // HEADER_GUARD_CORE__IDATAPROCESSOR_H__
