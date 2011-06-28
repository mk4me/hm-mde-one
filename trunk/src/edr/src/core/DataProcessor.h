/********************************************************************
    created:  2011/04/19
    created:  19:4:2011   16:45
    filename: DataProcessor.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__DATAPROCESSOR_H__
#define HEADER_GUARD_CORE__DATAPROCESSOR_H__

#include <core/IDataProcessor.h>
#include <core/ObjectWrapper.h>
#include "WorkflowItemEncapsulator.h"
#include "InputOutputItem.h"
#include "ObjectOutput.h"

class DataProcessor : public InputOutputItem<core::IDataProcessor>
{

public:
    //! \param impl Obiekt przejmowany na w³asnoœæ.
    DataProcessor(core::IDataProcessor* impl);
    ~DataProcessor();

public:

    virtual void run() 
    {
        getImplementation()->process(getSource(), getOutput());
    }

    static void test();

};

typedef boost::shared_ptr<DataProcessor> DataProcessorPtr;
typedef boost::shared_ptr<const DataProcessor> DataProcessorConstPtr;


#endif  // HEADER_GUARD_CORE__DATAPROCESSOR_H__