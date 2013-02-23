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
	int margin = 100;
	if (start.y() < finish.y()) {
		rect.setTop(start.y() - margin);
		rect.setBottom(finish.y() + margin);
	} else {
		rect.setTop(finish.y() - margin);
		rect.setBottom(start.y() + margin);
	}
	if (start.x() < finish.x()) {
		rect.setLeft(start.x() - margin);
		rect.setRight(finish.x() + margin);
	} else {
		rect.setLeft(finish.x() - margin);
		rect.setRight(start.x() + margin);
	}
	return rect;
}
