/********************************************************************
	created:	2011/05/12
	created:	12:5:2011   11:26
	filename: 	InputOutputItem.h
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__INPUTOUTPUTITEM_H__
#define HEADER_GUARD_CORE__INPUTOUTPUTITEM_H__

#include <core/IInputOutputProcessItem.h>
#include "WorkflowItemEncapsulator.h"
#include "InputDescription.h"
#include "OutputDescription.h"


template<class T>
class InputOutputItem : public WorkflowItemEncapsulator<T>, 
    public InputDescription, public OutputDescription
{
        UTILS_STATIC_ASSERT((boost::is_base_of<core::IInputOutputProcessItem, T>::value), "Template class should inherit from core::IInputOutputProcessItem");

public:
      InputOutputItem(const InputOutputItem& item) : 
      WorkflowItemEncapsulator<T>(item),
          InputDescription(item),
          OutputDescription(item)
      {}

      InputOutputItem( T* implementation ) : 
      WorkflowItemEncapsulator<T>(implementation),
          InputDescription(createSource()),
          OutputDescription(createOutput())
      {}

public:
    virtual void run()
    {
        getImplementation()->process(getSource(), getOutput());
    }
};

#endif
