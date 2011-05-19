/********************************************************************
	created:	2011/05/12
	created:	12:5:2011   11:27
	filename: 	InputItem.h
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__INPUTITEM_H__
#define HEADER_GUARD_CORE__INPUTITEM_H__

#include <boost/shared_ptr.hpp>
#include <core/WorkflowItemEncapsulator.h>
#include <core/IInputProcessItem.h>
#include <core/SmartPtr.h>
#include "Visualizer.h"

namespace core {

typedef Visualizer InputItem;
typedef boost::shared_ptr<InputItem> InputItemPtr;
typedef boost::shared_ptr<const InputItem> InputItemConstPtr;

}

#endif