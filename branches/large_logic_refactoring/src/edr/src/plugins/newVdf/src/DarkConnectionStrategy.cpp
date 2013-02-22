#include "DarkConnectionStrategy.h"
#include <plugins/newVdf/IVisualPin.h>

using namespace vdf;

DarkConnectionStrategy::DarkConnectionStrategy()
{

}

void DarkConnectionStrategy::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= nullptr */ )
{
	UTILS_ASSERT(beginPin.lock() && endPin.lock());
	painter->setRenderHint(QPainter::Antialiasing, true);

	QPainterPath path;
	QPointF start = beginPin.lock()->visualItem()->scenePos(); 
	QPointF finish = endPin.lock()->visualItem()->scenePos();
	// HACK +8
	if (start.x() < finish.x()) {
		path.moveTo(QPointF(start.x() + 8, start.y() + 8));
		path.cubicTo(QPointF(start.x() + 20, start.y()), QPointF(finish.x() - 20, finish.y()), QPointF(finish.x() + 8, finish.y() + 8) );
	} else {
		path.moveTo(QPointF(start.x() + 8, start.y() + 8));
		path.cubicTo(QPointF(start.x() - 20, start.y()), QPointF(finish.x() + 20, finish.y()), QPointF(finish.x() + 8, finish.y() + 8) );
	}

	painter->setPen(QPen(QColor(128, 213, 220), 4));
	painter->drawPath(path);
}

void DarkConnectionStrategy::setPins(const IVisualPinWeakPtr pin1, const IVisualPinWeakPtr pin2 )
{
	this->beginPin = pin1;
	this->endPin = pin2;
}

const QRectF& DarkConnectionStrategy::getRect()
{
	QPointF start = beginPin.lock()->visualItem()->scenePos(); 
	QPointF finish = endPin.lock()->visualItem()->scenePos();
	if (start.y() < finish.y()) {
		rect.setTop(start.y() - 20);
		rect.setBottom(finish.y() + 20);
	} else {
		rect.setTop(finish.y() - 20);
		rect.setBottom(start.y() + 20);
	}
	if (start.x() < finish.x()) {
		rect.setLeft(start.x() - 20);
		rect.setRight(finish.x() + 20);
	} else {
		rect.setLeft(finish.x() - 20);
		rect.setRight(start.x() + 20);
	}
	return rect;
}
