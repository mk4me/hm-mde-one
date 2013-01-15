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
#include "InputOutputItem.h"
#include "ObjectOutput.h"

namespace core {

class DataProcessor : public InputOutputItem<plugin::IDataProcessor>
{

public:
    //! \param impl Obiekt przejmowany na własność.
    DataProcessor(plugin::IDataProcessor* impl);
    ~DataProcessor();

public:

    virtual void run() 
    {
        //getImplementation()->process(getSource(), getOutput());
    }

    //static void test();

};

typedef shared_ptr<DataProcessor> DataProcessorPtr;
typedef shared_ptr<const DataProcessor> DataProcessorConstPtr;

}

#endif  // HEADER_GUARD_CORE__DATAPROCESSOR_H__
