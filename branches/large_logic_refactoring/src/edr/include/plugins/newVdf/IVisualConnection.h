/********************************************************************
    created:  2012/11/15
    created:  15:11:2012   9:44
    filename: IVisualConnection.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IVISUALCONNECTION_H__
#define HEADER_GUARD___IVISUALCONNECTION_H__

#include <dflib/IConnection.h>
#include <plugins/newVdf/IVisualItem.h>
#include <plugins/newVdf/IVisualOutputPin.h>
#include <plugins/newVdf/IVisualInputPin.h>
#include <utils/Debug.h>

namespace df { class IConnection; }

namespace vdf {
    
class IVisualConnection : public IVisualItem
{
public:
	IVisualConnection() {}
	virtual ~IVisualConnection() {
    }

public:
	virtual void setInputPin(IVisualInputPinPtr pin) = 0;
	virtual void setOutputPin(IVisualOutputPinPtr pin) = 0;
	virtual IVisualInputPinPtr getInputPin() = 0;
	virtual IVisualOutputPinPtr getOutputPin() = 0;
	// TODO: przeniesc implementacje
	df::IConnection* getModelConnection() {  UTILS_ASSERT(_connection); return _connection.get(); }
	const df::IConnection* getModelConnection() const { UTILS_ASSERT(_connection); return _connection.get(); }
	void setModelConnection(df::IConnection* node) { _connection.reset(node); }

	virtual IVisualItem::Type getType() const { return IVisualItem::Connection; }
	virtual bool isType(IVisualItem::Type t) const { return t == IVisualItem::Connection; }

    virtual bool isSelected() const = 0;
    virtual void setSelected(bool val) = 0;

private:
	core::shared_ptr<df::IConnection> _connection;
};
typedef core::shared_ptr<IVisualConnection> IVisualConnectionPtr;
typedef core::weak_ptr<IVisualConnection> IVisualConnectionWeakPtr;
typedef core::shared_ptr<const IVisualConnection> IVisualConnectionConstPtr;

}

#endif	//	HEADER_GUARD___IVISUALCONNECTION_H__
