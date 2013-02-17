/********************************************************************
	created:	2013/02/07
	created:	7:2:2013   13:18
	filename: 	DarkConnectionStrategy.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__DARKCONNECTIONSTRATEGY_H__
#define HEADER_GUARD_NEWVDF__DARKCONNECTIONSTRATEGY_H__

#include <plugins/newVdf/IConnectionStrategy.h>
#include <plugins/newVdf/IVisualPin.h>
class IVisualOutputPin;
class IVisualInputPin;
class DarkConnectionStrategy : public IConnectionStrategy
{
public:
	DarkConnectionStrategy();
	virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr );
	virtual void setPins(IVisualPinWeakPtr pin1, IVisualPinWeakPtr pin2);
	virtual const QRectF& getRect();

private:
	IVisualPinWeakPtr beginPin;
	IVisualPinWeakPtr endPin;
	QRectF rect;
};


typedef core::shared_ptr<DarkConnectionStrategy> DarkConnectionStrategyPtr;
typedef core::shared_ptr<const DarkConnectionStrategy> DarkConnectionStrategyConstPtr;


#endif
