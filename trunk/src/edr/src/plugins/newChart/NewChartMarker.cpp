#include "NewChartPCH.h"
#include "NewChartMarker.h"
#include <QtGui/QPainter>

NewChartMarker::NewChartMarker( ScalarChannelReaderInterfaceConstPtr reader ) :
reader(reader)
{

}


void NewChartMarker::drawLines( QPainter *painter, const QRectF &rect, const QPointF & point ) const
{
    QwtPlotMarker::drawLines(painter, rect, point);
}

float lerp(float from, float to, float ratio)
{
    return from * (1.0f - ratio) + to * ratio;
}

void NewChartMarker::drawLabel( QPainter *painter, const QRectF &rect, const QPointF & point ) const
{
   painter->setPen(QPen(QColor(135, 173, 255)));
   painter->setBrush(QBrush(QColor(65, 113, 195, 128)));
   painter->drawEllipse(point, 5, 5);

   static float rectY = point.y() + 20;

   rectY = lerp(rectY, point.y() + 20, 0.1f);
   
   painter->setBrush(QBrush(QColor(255,255,255)));
   painter->drawLine(point.x() + 3, point.y() + 3, point.x() + 20, static_cast<int>(rectY));
   
   QRect textRect;
   textRect.setX(point.x() + 20);
   textRect.setY(static_cast<int>(rectY));
   textRect.setWidth(90);
   textRect.setHeight(35);
   
   int bound = 3;
   QRect boxRect;
   boxRect.setX(textRect.x() - bound);
   boxRect.setY(textRect.y() - bound);
   boxRect.setWidth(textRect.width() + bound * 2);
   boxRect.setHeight(textRect.height() + bound * 2);
   
   painter->drawRoundedRect(boxRect, 6, 6);
   QString text = QString("Time: %1\nValue: %2").arg(xValue()).arg(yValue());

   painter->setPen(QPen(QColor(100, 100, 100)));
   painter->drawText(textRect, Qt::AlignCenter, text);
}



