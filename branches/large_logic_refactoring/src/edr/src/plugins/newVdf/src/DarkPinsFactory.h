/********************************************************************
	created:	2013/02/08
	created:	8:2:2013   16:17
	filename: 	DarkPinsFactory.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__DARKPINSFACTORY_H__
#define HEADER_GUARD_NEWVDF__DARKPINSFACTORY_H__

#include <plugins/newVdf/IPinsFactory.h>

class DarkPinsFactory : public IPinsFactory
{
public:
	virtual ~DarkPinsFactory() {}

public:
	virtual IVisualInputPinPtr createInputPin() const;
	virtual IVisualOutputPinPtr createOutputPin() const;
	virtual const QUuid id() const;
	virtual const QString name() const;
	virtual const QString description() const;
};
typedef core::shared_ptr<DarkPinsFactory> DarkPinsFactoryPtr;
typedef core::shared_ptr<const DarkPinsFactory> DarkPinsFactoryConstPtr;

//class ConnectionsFactory : public IConnectionsFactory
//{
//public:
//	virtual ~ConnectionsFactory() {}
//
//    virtual IVisualConnection * createConnection() const;
//
//    virtual const QUuid id() const;
//
//    virtual const QString name() const;
//
//    virtual const QString description() const;
//
//};
//typedef core::shared_ptr<ConnectionsFactory> ConnectionFactoryPtr;
//typedef core::shared_ptr<const ConnectionsFactory> ConnectionFactoryConstPtr;
//
//class StyleConnectionsFactory : public IConnectionsFactory
//{
//public:
//	virtual ~StyleConnectionsFactory() {}
//
//	virtual IVisualConnection * createConnection() const;
//
//	virtual const QUuid id() const;
//
//	virtual const QString name() const;
//
//	virtual const QString description() const;
//
//};
//typedef core::shared_ptr<StyleConnectionsFactory> StyleConnectionsFactoryPtr;
//typedef core::shared_ptr<const StyleConnectionsFactory> StyleConnectionsFactoryConstPtr;
//
//
//class IVisualOutputPin;
//class IVisualInputPin;
//class ConnectionStrategy : public IConnectionStrategy
//{
//public:
//	ConnectionStrategy();
//	virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr );
//	virtual void setPins(const IVisualPin* pin1, const IVisualPin* pin2);
//	virtual const QRectF& getRect();
//	
//private:
//	const IVisualPin* beginPin;
//	const IVisualPin* endPin;
//	QRectF rect;
//};


#endif
