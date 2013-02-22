#include "DarkPinStrategy.h"

using namespace vdf;

DarkPinStrategy::DarkPinStrategy()
{
	setState(IPinStrategy::Normal);
}

void DarkPinStrategy::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= nullptr */ )
{
	painter->setRenderHint(QPainter::Antialiasing);
	painter->setPen(QPen(QColor(128, 213, 220)));
	painter->setBrush(QBrush(QColor(128, 213, 220)));

	QRectF r = getRect();
	painter->drawEllipse(r);

	QRadialGradient grad(6.0f,6.0f,8.0f);
	grad.setColorAt(0.0f, Qt::white);
	grad.setColorAt(0.5f, stateColor);
	//painter->setBrush(QBrush(QColor(25, 170, 220)));
	painter->setBrush(grad);
	painter->setPen(QPen(QColor(100, 100, 100)));


	float width = r.width() * 0.7f;
	float height = r.height() * 0.7f;
	float x = (r.width() - width) * 0.5f;
	float y = (r.height() - height) * 0.5f;
	painter->drawEllipse(x, y, width, height);
}

void DarkPinStrategy::setPin( IVisualPinWeakPtr item )
{
	UTILS_ASSERT(item.lock());
	pin = item;
}

const QRectF& DarkPinStrategy::getRect()
{
	rect.setRect(0, 0, 16, 16);
	return rect;
}

void DarkPinStrategy::setState( State s )
{
	switch(s) {
	case IPinStrategy::Connected:
		stateColor = QColor(128, 213, 220);
		break;

	case IPinStrategy::Connective:
		stateColor = QColor(220, 220, 220);
		break;

	case IPinStrategy::Normal:
		stateColor = QColor(25, 170, 220);
		break;

	case IPinStrategy::Unconnective:
		stateColor = QColor(250, 170, 22);
		break;

	default:
		UTILS_ASSERT(false);
	}
}

