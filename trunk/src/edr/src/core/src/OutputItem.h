/********************************************************************
	created:	2011/05/12
	created:	12:5:2011   11:26
	filename: 	OutputItem.h

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_CORE__OUTPUTITEM_H__
#define HEADER_GUARD_CORE__OUTPUTITEM_H__

#include "WorkflowItemEncapsulator.h"
#include "OutputDescription.h"
#include <core/IInputProcessItem.h>
#include <core/IOutputProcessItem.h>

template<class T>
class OutputItem : public WorkflowItemEncapsulator<T> , public OutputDescription
{
    UTILS_STATIC_ASSERT((boost::is_base_of<core::IOutputProcessItem, T>::value), "Template class should inherit from core::IOutputProcessItem");
public:
    OutputItem(T* implementation, const ObjectOutput& output) :
      WorkflowItemEncapsulator<T>(implementation),
          OutputDescription(output)
      {}

      OutputItem(const OutputItem& item) :
      WorkflowItemEncapsulator<T>(item),
          OutputDescription(createOutput())
      {}

      OutputItem( T* implementation ) :
      WorkflowItemEncapsulator<T>(implementation),
          OutputDescription(createOutput())
      {}
public:
    virtual void run()
    {
        getImplementation()->generate(getOutput());
    }
};


#endif
