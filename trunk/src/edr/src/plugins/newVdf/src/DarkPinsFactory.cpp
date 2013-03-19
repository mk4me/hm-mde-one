#include "NewVdfPCH.h"
#include "DarkPinsFactory.h"
#include "DarkPinStrategy.h"
#include "StyleInputPinNode.h"
#include "StyleOutputPinNode.h"
#include <plugins/newVdf/IVisualPin.h>

using namespace vdf;

IVisualInputPinPtr DarkPinsFactory::createInputPin() const
{
	auto pin = IVisualInputPinPtr(new StyleInputPinNode());
	auto str = IPinStrategyPtr(new DarkPinStrategy());
	str->setPin(pin);
	pin->setVisualStrategy(str);
	return pin;
}
	
IVisualOutputPinPtr DarkPinsFactory::createOutputPin() const
{
	auto pin = IVisualOutputPinPtr(new StyleOutputPinNode());
	auto str = IPinStrategyPtr(new DarkPinStrategy());
	str->setPin(pin);
	pin->setVisualStrategy(str);
	return pin;
}
	
const QUuid DarkPinsFactory::id() const
{
	return QUuid::createUuid();
}
	
const QString DarkPinsFactory::name() const
{
	return "Style pin";
}
	
const QString DarkPinsFactory::description() const
{
	return "Style pin desc";
}

//
//IVisualConnection * ConnectionsFactory::createConnection() const
//{
//    return new SimpleConnection();
//}
//
//const QUuid ConnectionsFactory::id() const
//{
//    return QUuid::createUuid();
//}
//
//const QString ConnectionsFactory::name() const
//{
//    return "Simple";
//}
//
//const QString ConnectionsFactory::description() const
//{
//    return "Simple line";
//}
//
//IVisualConnection * StyleConnectionsFactory::createConnection() const
//{
//    auto connection = new StyleConnection();
//	auto strategy = IConnectionStrategyPtr(new DarkConnectionStrategy());
//	connection->setVisualStrategy(strategy);
//	return connection;
//}
//
//const QUuid StyleConnectionsFactory::id() const
//{
//    return QUuid::createUuid();
//}
//
//const QString StyleConnectionsFactory::name() const
//{
//    return "Curved";
//}
//
//const QString StyleConnectionsFactory::description() const
//{
//    return "Curved line";
//}
//
//ConnectionStrategy::ConnectionStrategy() : 
//	endPin(nullptr),
//	beginPin(nullptr)
//{
//
//}
//
//void ConnectionStrategy::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= nullptr */ )
//{
//	UTILS_ASSERT(beginPin && endPin);
//	painter->setRenderHint(QPainter::Antialiasing, true);
//
//	QPainterPath path;
//	QPointF start = beginPin->visualItem()->scenePos(); 
//	QPointF finish = endPin->visualItem()->scenePos();
//	// HACK +8
//	if (start.x() < finish.x()) {
//		path.moveTo(QPointF(start.x() + 8, start.y() + 8));
//		path.cubicTo(QPointF(start.x() + 20, start.y()), QPointF(finish.x() - 20, finish.y()), QPointF(finish.x() + 8, finish.y() + 8) );
//	} else {
//		path.moveTo(QPointF(start.x() + 8, start.y() + 8));
//		path.cubicTo(QPointF(start.x() - 20, start.y()), QPointF(finish.x() + 20, finish.y()), QPointF(finish.x() + 8, finish.y() + 8) );
//	}
//
//	painter->setPen(QPen(QColor(128, 213, 220), 4));
//	painter->drawPath(path);
//	//painter->drawLine(endPin->visualItem()->scenePos(), beginPin->visualItem()->scenePos());
//}
//
//void ConnectionStrategy::setPins(const IVisualPin* pin1, const IVisualPin* pin2 )
//{
//	this->beginPin = pin1;
//	this->endPin = pin2;
//}
//
//const QRectF& ConnectionStrategy::getRect()
//{
//	QPointF start = beginPin->visualItem()->scenePos(); 
//	QPointF finish = endPin->visualItem()->scenePos();
//	if (start.y() < finish.y()) {
//		rect.setTop(start.y() - 20);
//		rect.setBottom(finish.y() + 20);
//	} else {
//		rect.setTop(finish.y() - 20);
//		rect.setBottom(start.y() + 20);
//	}
//	if (start.x() < finish.x()) {
//		rect.setLeft(start.x() - 20);
//		rect.setRight(finish.x() + 20);
//	} else {
//		rect.setLeft(finish.x() - 20);
//		rect.setRight(start.x() + 20);
//	}
//	return rect;
//}
