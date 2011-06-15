/********************************************************************
    created:  2011/04/14
    created:  14:4:2011   17:29
    filename: IDataProcessor.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__IDATAPROCESSOR_H__
#define HEADER_GUARD_CORE__IDATAPROCESSOR_H__

#include <core/ObjectWrapper.h>
#include <core/IObjectSource.h>
#include <core/IObjectOutput.h>
#include <core/IInputOutputProcessItem.h>
#include <list>

namespace core 
{
    typedef IInputOutputProcessItem IDataProcessor;

    typedef shared_ptr<IDataProcessor> IDataProcessorPtr;
    typedef shared_ptr<const IDataProcessor> IDataProcessorConstPtr;
} // namespace core


#endif  // HEADER_GUARD_CORE__IDATAPROCESSOR_H__