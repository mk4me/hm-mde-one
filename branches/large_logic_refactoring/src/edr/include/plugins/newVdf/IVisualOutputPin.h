/********************************************************************
	created:	2013/02/10
	created:	10:2:2013   20:20
	filename: 	IVisualOutputPin.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__IVISUALOUTPUTPIN_H__
#define HEADER_GUARD_NEWVDF__IVISUALOUTPUTPIN_H__

#include <plugins/newVdf/IVisualPin.h>

namespace vdf {

class IVisualOutputPin : public IVisualPin
{
public:

	virtual void markIncomplete() = 0;
	virtual Type getType() const { return IVisualItem::OutputPin; }
	virtual bool isType(Type t) const { return t == IVisualItem::OutputPin || t == IVisualItem::Pin; }
};
typedef core::shared_ptr<IVisualOutputPin> IVisualOutputPinPtr;
typedef core::weak_ptr<IVisualOutputPin> IVisualOutputPinWeakPtr;
typedef core::shared_ptr<const IVisualOutputPin> IVisualOutputPinConstPtr;

}

#endif
