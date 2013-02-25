#include "DarkConnectionStrategy.h"
#include <plugins/newVdf/IVisualPin.h>

using namespace vdf;

DarkConnectionStrategy::DarkConnectionStrategy()
{

}

void DarkConnectionStrategy::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= nullptr */ )
{
    auto c = connection.lock();
	if (c && hasPins()) {
        
	    painter->setRenderHint(QPainter::Antialiasing, true);

	    //QPainterPath path;
        path = QPainterPath();
	    QPointF start = beginPin.lock()->visualItem()->scenePos(); 
	    QPointF finish = endPin.lock()->visualItem()->scenePos();
        int shift = 30;
	    // HACK +8
	    if (start.x() < finish.x()) {
		    path.moveTo(QPointF(start.x() + 8, start.y() + 8));
		    path.cubicTo(QPointF(start.x() + shift, start.y()), QPointF(finish.x() - shift, finish.y()), QPointF(finish.x() + 8, finish.y() + 8) );
	    } else {
		    path.moveTo(QPointF(start.x() + 8, start.y() + 8));
		    path.cubicTo(QPointF(start.x() - shift, start.y()), QPointF(finish.x() + shift, finish.y()), QPointF(finish.x() + 8, finish.y() + 8) );
	    }

	    painter->setPen(QPen(QColor(128, 213, 220), c->isSelected() ? 4 : 2));
	    painter->drawPath(path);

    }
}

//void DarkConnectionStrategy::setPins(const IVisualPinWeakPtr pin1, const IVisualPinWeakPtr pin2 )
//{
//	this->beginPin = pin1;
//	this->endPin = pin2;
//}

const QRectF& DarkConnectionStrategy::getRect()
{
    if (hasPins()) {
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
    }
	return rect;
}

QPainterPath vdf::DarkConnectionStrategy::shape() const
{
    return path;
}

void vdf::DarkConnectionStrategy::setConnection( core::weak_ptr<IVisualConnection> connection )
{
    this->connection = connection;
}

bool vdf::DarkConnectionStrategy::hasPins()
{
    auto p1 = beginPin.lock();
    auto p2 = endPin.lock();
    if (!p2 || !p1) {
        auto c = connection.lock();
        beginPin = c->getInputPin();
        endPin = c->getOutputPin();
        p1 = beginPin.lock();
        p2 = beginPin.lock();
    }

    return p1 && p2;
}
