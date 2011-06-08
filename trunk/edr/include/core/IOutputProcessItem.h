/********************************************************************
	created:	2011/05/12
	created:	12:5:2011   9:13
	filename: 	IOutputProcessItem.h
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__IOUTPUTPROCESSITEM_H__
#define HEADER_GUARD_CORE__IOUTPUTPROCESSITEM_H__

#include <core/IOutputDescription.h>
#include <core/IWorkflowItemBase.h>
#include <core/IObjectOutput.h>

class QDialog;

namespace core {

class IOutputProcessItem : public IOutputDescription, public IWorkflowItemBase
{
public:
    virtual void generate(IObjectOutput* output) = 0;
    virtual ~IOutputProcessItem() {}
};

}

#endif