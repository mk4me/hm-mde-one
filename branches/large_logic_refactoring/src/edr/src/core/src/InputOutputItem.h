/********************************************************************
	created:	2011/05/12
	created:	12:5:2011   11:26
	filename: 	InputOutputItem.h
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__INPUTOUTPUTITEM_H__
#define HEADER_GUARD_CORE__INPUTOUTPUTITEM_H__

#include <core/IInputDescription.h>
#include <core/IOutputDescription.h>
#include "InputDescription.h"
#include "OutputDescription.h"

namespace core {

template<class T>
class InputOutputItem : public InputDescription, public OutputDescription
{
        UTILS_STATIC_ASSERT((boost::is_base_of<plugin::IOutputDescription, T>::value), "Template class should inherit from plugin::IOutputDescription");
		UTILS_STATIC_ASSERT((boost::is_base_of<plugin::IInputDescription, T>::value), "Template class should inherit from plugin::IInputDescription");

public:

	//TODO
	//do usuniêcia
	InputOutputItem() {}

      InputOutputItem(const InputOutputItem& item) : 
      //WorkflowItemEncapsulator<T>(item),
          InputDescription(item),
          OutputDescription(item)
      {}

      InputOutputItem( T* implementation ) //: 
      //WorkflowItemEncapsulator<T>(implementation),
			//TODO
          //InputDescription(createSource()),
          //OutputDescription(createOutput())
      {}

public:
    virtual void run()
    {
        //getImplementation()->process(getSource(), getOutput());
    }
};

}

#endif
