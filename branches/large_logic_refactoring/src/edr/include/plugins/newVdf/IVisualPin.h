/********************************************************************
    created:  2012/11/15
    created:  15:11:2012   9:35
    filename: IVisualPin.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IVISUALPIN_H__
#define HEADER_GUARD___IVISUALPIN_H__

//#include <dflib/Pin.h>
#include <plugins/newVdf/IVisualItem.h>

namespace vdf {

class IVisualNode;
class IVisualConnection;
class IVisualPin : public IVisualItem
{
protected:
	IVisualPin() {}

public:
	virtual ~IVisualPin() {}

	virtual void markNormal() = 0;
	virtual void markCompatible() = 0;
	virtual void markIncompatible() = 0;
	virtual void markConnective() = 0;
	virtual void markUnconnective() = 0;
	virtual void markConnected() = 0;
	virtual int getIndex() const = 0;
	virtual void setIndex(int val) = 0;
    virtual QPointF getConnectionPosHint() = 0;
	virtual core::weak_ptr<IVisualNode> getParent() const = 0;
	virtual void setParent(core::weak_ptr<IVisualNode> val) = 0;
	//virtual void setConnection(core::weak_ptr<IVisualConnection> connection) = 0;
	//virtual core::weak_ptr<IVisualConnection> getConnection() const = 0;
};
typedef core::shared_ptr<IVisualPin> IVisualPinPtr;
typedef core::weak_ptr<IVisualPin> IVisualPinWeakPtr;
typedef core::shared_ptr<const IVisualPin> IVisualPinConstPtr;

}

#endif	//	HEADER_GUARD___IVISUALPIN_H__
