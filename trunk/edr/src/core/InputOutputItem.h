/********************************************************************
	created:	2011/05/12
	created:	12:5:2011   11:26
	filename: 	InputOutputItem.h
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__INPUTOUTPUTITEM_H__
#define HEADER_GUARD_CORE__INPUTOUTPUTITEM_H__

#include <core/WorkflowItemEncapsulator.h>
#include <core/IDataProcessor.h>
#include "DataProcessor.h"

namespace core {

typedef DataProcessor InputOutputItem;
typedef boost::shared_ptr<InputOutputItem> InputOutputItemPtr;
typedef boost::shared_ptr<const InputOutputItem> InputOutputItemConstPtr;

}

#endif