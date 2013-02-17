/********************************************************************
    created:  2012/11/15
    created:  15:11:2012   9:44
    filename: IVisualConnection.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IVISUALCONNECTION_H__
#define HEADER_GUARD___IVISUALCONNECTION_H__

#include <plugins/newVdf/IVisualItem.h>
#include <plugins/newVdf/IVisualPin.h>
#include <utils/Debug.h>


class IVisualPin;
namespace df { class IConnection; }

class IVisualConnection : public IVisualItem
{
public:
	IVisualConnection() : _connection(nullptr) {}
	virtual ~IVisualConnection() {}

public:
	virtual void setBegin(IVisualPinPtr pin) = 0;
	virtual void setEnd(IVisualPinPtr pin) = 0;
	virtual IVisualPinPtr getBegin() = 0;
	virtual IVisualPinPtr getEnd() = 0;
	// TODO: przeniesc implementacje
	df::IConnection* getModelConnection() {  UTILS_ASSERT(_connection); return _connection; }
	const df::IConnection* getModelConnection() const { UTILS_ASSERT(_connection); return _connection; }
	void setModelConnection(df::IConnection* node) { _connection = node; }

	virtual IVisualItem::Type getType() const { return IVisualItem::Connection; }
	virtual bool isType(IVisualItem::Type t) const { return t == IVisualItem::Connection; }

private:
	df::IConnection* _connection;
};
typedef core::shared_ptr<IVisualConnection> IVisualConnectionPtr;
typedef core::weak_ptr<IVisualConnection> IVisualConnectionWeakPtr;
typedef core::shared_ptr<const IVisualConnection> IVisualConnectionConstPtr;

#endif	//	HEADER_GUARD___IVISUALCONNECTION_H__
