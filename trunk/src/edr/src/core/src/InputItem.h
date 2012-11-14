/********************************************************************
	created:	2011/05/12
	created:	12:5:2011   11:27
	filename: 	InputItem.h
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__INPUTITEM_H__
#define HEADER_GUARD_CORE__INPUTITEM_H__

#include "WorkflowItemEncapsulator.h"
#include <core/IInputProcessItem.h>
#include "InputDescription.h"


template<class T>
class InputItem : public WorkflowItemEncapsulator<T> , public InputDescription
{
    UTILS_STATIC_ASSERT((boost::is_base_of<core::IInputProcessItem, T>::value), "Template class should inherit from core::IInputProcessItem");
public:
    InputItem(T* implementation, const ObjectSource& source) : 
      WorkflowItemEncapsulator<T>(implementation),
      InputDescription(source)
      {}
    InputItem(const InputItem& item) : 
      WorkflowItemEncapsulator<T>(item),
      InputDescription(item) 
    {}
public:
    virtual void run()
    {
        getImplementation()->setUp(getSource());
    }
};


#endif
