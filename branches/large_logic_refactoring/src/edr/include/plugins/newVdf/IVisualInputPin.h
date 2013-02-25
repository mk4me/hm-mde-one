/********************************************************************
	created:	2013/02/10
	created:	10:2:2013   20:20
	filename: 	IVisualInputPin.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__IVISUALINPUTPIN_H__
#define HEADER_GUARD_NEWVDF__IVISUALINPUTPIN_H__

#include <plugins/newVdf/IVisualPin.h>

namespace vdf {

class IVisualInputPin : public IVisualPin
{
public:
	virtual void markRequired() = 0;
	virtual void markMissing() = 0;
	virtual Type getType() const { return IVisualItem::InputPin; }
	virtual bool isType(Type t) const { return t == IVisualItem::InputPin || t == IVisualItem::Pin; }
    virtual core::weak_ptr<IVisualConnection> getConnection() const = 0;
    virtual void setConnection(core::weak_ptr<IVisualConnection> connection) = 0;
};
typedef core::shared_ptr<IVisualInputPin> IVisualInputPinPtr;
typedef core::weak_ptr<IVisualInputPin> IVisualInputPinWeakPtr;
typedef core::shared_ptr<const IVisualInputPin> IVisualInputPinConstPtr;

}

#endif
