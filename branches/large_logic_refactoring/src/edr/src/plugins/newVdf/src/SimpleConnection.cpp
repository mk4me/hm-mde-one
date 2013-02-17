#include "NewVdfPCH.h"
#include "SimpleConnection.h"
#include "SimpleItem.h"

SimpleConnection::SimpleConnection() :
    item(new QGraphicsLineItem())
{

}

void SimpleConnection::setBegin( const IVisualPinPtr pin )
{
    UTILS_ASSERT(!begin);
    begin = pin;
    QGraphicsObject* object = dynamic_cast<QGraphicsObject*>(pin->visualItem());
    UTILS_ASSERT(object);
    QObject::connect(object, SIGNAL(transformChanged()), this, SLOT(update()));
    update();
}

void SimpleConnection::setEnd( const IVisualPinPtr pin )
{
    UTILS_ASSERT(!end);
    end = pin;
    connect(dynamic_cast<QGraphicsObject*>(pin->visualItem()), SIGNAL(transformChanged()), this, SLOT(update()));
    update();
}

QGraphicsItem * SimpleConnection::visualItem() const
{
    return item;
}

void SimpleConnection::update()
{
    if (begin && end) {
        auto p1 = begin->visualItem()->scenePos();
        auto p2 = end->visualItem()->scenePos();
        item->setLine(p1.x(), p1.y(), p2.x(), p2.y());
        item->update();
    }
}

void SimpleConnection::setVisualStrategy( IVisualStrategyPtr strategy )
{
	UTILS_ASSERT(false);
}

IVisualPinPtr SimpleConnection::getBegin()
{
	return begin;
}

IVisualPinPtr SimpleConnection::getEnd()
{
	return end;
}

