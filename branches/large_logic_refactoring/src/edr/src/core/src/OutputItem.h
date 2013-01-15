/********************************************************************
	created:	2011/05/12
	created:	12:5:2011   11:26
	filename: 	OutputItem.h

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_CORE__OUTPUTITEM_H__
#define HEADER_GUARD_CORE__OUTPUTITEM_H__

#include "OutputDescription.h"
#include <core/IInputProcessItem.h>
#include <core/IOutputProcessItem.h>

namespace core {

template<class T>
class OutputItem : public OutputDescription
{
    UTILS_STATIC_ASSERT((boost::is_base_of<plugin::IOutputDescription, T>::value), "Template class should inherit from plugin::IOutputDescription");
public:
    OutputItem(T* implementation, const ObjectOutput& output) :
      //WorkflowItemEncapsulator<T>(implementation),
          OutputDescription(output)
      {}

      OutputItem(const OutputItem& item) :
      //WorkflowItemEncapsulator<T>(item),
          OutputDescription(createOutput())
      {}

      OutputItem( T* implementation ) :
      //WorkflowItemEncapsulator<T>(implementation),
          OutputDescription(createOutput())
      {}
public:
    virtual void run()
    {
        //getImplementation()->generate(getOutput());
    }
};

}

#endif
