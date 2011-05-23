/********************************************************************
	created:	2011/05/12
	created:	12:5:2011   9:12
	filename: 	IInputOutputProcessItem.h
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__IINPUTOUTPUTPROCESSITEM_H__
#define HEADER_GUARD_CORE__IINPUTOUTPUTPROCESSITEM_H__

#include <core/IWorkflowItemBase.h>
#include <core/IInputDescription.h>
#include <core/IOutputDescription.h>
#include <core/IObjectSource.h>
#include <core/IObjectOutput.h>

namespace core {

class IInputOutputProcessItem : public IWorkflowItemBase, public IInputDescription, public IOutputDescription
{
public:
    //! Przetwarza informacje.
    virtual void process(IObjectSource* input, IObjectOutput* output) = 0;
};

}

#endif