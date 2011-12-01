#include "NewChartPCH.h"
#include "NewChartMarker.h"
#include <utils/Utils.h>
#include <QtGui/QPainter>

const int LABEL_WIDTH = 90;
const int LABEL_HEIGHT = 35;
const int LABEL_PDIST = 20;
const int LABEL_BOUND = 3;
const float MIN_LERP_DIST = 15.0f;
const float LERP_Y = 0.1f;
const float LERP_X = 0.33f;

NewChartMarker::NewChartMarker( ScalarChannelReaderInterfaceConstPtr reader ) :
    reader(reader),
    positionSet(false),
    lerpX(LERP_X),
    lerpY(LERP_Y)
{
    setZ(1001);
}

NewChartMarker::NewChartMarker() :
positionSet(false),
    lerpX(LERP_X),
    lerpY(LERP_Y)
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
    drawDot(painter, point);


    int px = point.x();
    int py = point.y();

    if (px < rect.left() || px > rect.right() || py > rect.bottom() || py < rect.top()) {
        resetMomentum();
        return;
    }

    float destX = px + LABEL_PDIST;
    float destY = py + LABEL_PDIST;

    if ((destX + LABEL_WIDTH) > rect.right()) {
        destX = px - LABEL_PDIST - LABEL_WIDTH;
    }

    if ((destY + LABEL_HEIGHT) > rect.bottom()) {
        destY = py - LABEL_PDIST - LABEL_HEIGHT;
    }

    if (!positionSet) {
        position.setX(px + LABEL_PDIST);
        position.setY(py + LABEL_PDIST);
        positionSet = true;
    }
    
    
    if (abs(position.x() - destX) > MIN_LERP_DIST) {
        position.setX(lerp(position.x(), destX, lerpX));
    }

    if (abs(position.y() - destY) > MIN_LERP_DIST) {
       position.setY(lerp(position.y(), destY, lerpY)); 
    }
    
    

    painter->setPen(QPen(QColor(135, 173, 255)));
    painter->setBrush(QBrush(QColor(255,255,255)));

    int lineX = utils::clamp(point.x(), position.x(), position.x() + LABEL_WIDTH);
    int lineY = utils::clamp(point.y(), position.y(), position.y() + LABEL_HEIGHT);

    painter->drawLine(point.x() + 3, point.y() + 3, lineX, lineY);
   
    QRect textRect;
    textRect.setX(static_cast<int>(position.x()));
    textRect.setY(static_cast<int>(position.y()));
    textRect.setWidth(LABEL_WIDTH);
    textRect.setHeight(LABEL_HEIGHT);
    
    QRect boxRect;
    boxRect.setX(textRect.x() - LABEL_BOUND);
    boxRect.setY(textRect.y() - LABEL_BOUND);
    boxRect.setWidth(textRect.width() + LABEL_BOUND * 2);
    boxRect.setHeight(textRect.height() + LABEL_BOUND * 2);
   
    painter->drawRoundedRect(boxRect, 6, 6);
    QString text = QString("Time: %1\nValue: %2").arg(xValue()).arg(yValue());

    painter->setPen(QPen(QColor(100, 100, 100)));
    painter->drawText(textRect, Qt::AlignCenter, text);
}

void NewChartMarker::drawDot( QPainter * painter, const QPointF & point, int size )
{
    painter->setPen(QPen(QColor(Qt::black)));
    painter->drawEllipse(point, 1, 1);

    painter->setPen(QPen(QColor(135, 173, 255)));
    
    QBrush b;
    b.setStyle(Qt::NoBrush);
    //painter->setBrush(QBrush(QColor(65, 113, 195, 128)));
    painter->setBrush(b);
    painter->drawEllipse(point, size, size);
}



NewChartLabel::NewChartLabel(const QString& text, const QPoint& shift, const QPoint& size) :
    text(text),
    size(size),
    shift(shift),
    pen(QColor(135, 173, 255)),
    brush(QColor(Qt::white))
{
    setZ( 1000 );
    brush.setStyle(Qt::SolidPattern);
}


void NewChartLabel::draw( QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRectF &canvasRect ) const
{
    UTILS_ASSERT(point1);
    QPoint transformed1 = QwtScaleMap::transform(xMap, yMap, point1->getPosition()).toPoint();
    
    
    

    QRect textRect;
    textRect.setX(transformed1.x() + shift.x());
    textRect.setY(transformed1.y() + shift.y());
    textRect.setWidth(LABEL_WIDTH);
    textRect.setHeight(LABEL_HEIGHT);

    QRect boxRect;
    boxRect.setX(textRect.x() - LABEL_BOUND);
    boxRect.setY(textRect.y() - LABEL_BOUND);
    boxRect.setWidth(textRect.width() + LABEL_BOUND * 2);
    boxRect.setHeight(textRect.height() + LABEL_BOUND * 2);

    /*QBrush b(pen.color());
    b.setStyle(Qt::SolidPattern);
    painter->setBrush(b);*/
    painter->setPen(QPen(Qt::black));
    painter->setBrush(QBrush(Qt::black));
    if (!point2) {
        drawConnection(painter, boxRect, transformed1, connectionStyle);
    } else {
        QPoint transformed2 = QwtScaleMap::transform(xMap, yMap, point2->getPosition()).toPoint();

        if (connectionStyle == Horizontal) {
            if (transformed1.x() > transformed2.x()) {
                QPoint temp = transformed1;
                transformed1 = transformed2;
                transformed2 = temp;
            }

            drawConnection(painter, boxRect, transformed1, connectionStyle, boxRect.left() > transformed2.x());
            drawConnection(painter, boxRect, transformed2,  connectionStyle, boxRect.right() < transformed1.x());
        } else if (connectionStyle == Vertical) {
            if (transformed1.y() > transformed2.y()) {
                QPoint temp = transformed1;
                transformed1 = transformed2;
                transformed2 = temp;
            }
            drawConnection(painter, boxRect, transformed1, connectionStyle, boxRect.top() > transformed2.y());
            drawConnection(painter, boxRect, transformed2,  connectionStyle, boxRect.bottom() < transformed1.y());
        } else {
            UTILS_ASSERT(false);
        }
    }

    painter->setPen(pen);
    painter->setBrush(brush);
    painter->drawRoundedRect(boxRect, 6, 6);
        
    painter->setPen(QPen(QColor(100, 100, 100)));
    painter->drawText(textRect, Qt::AlignCenter, text);
}

bool NewChartLabel::isInsideLabel( const QPoint& transformedPoint, const QwtPlotCurve* curve ) const
{
   QPoint pointingTransformed = getPoint1Transformed(curve);
   QRect rect;
   rect.setTopLeft(pointingTransformed + shift);
   rect.setWidth(size.x());
   rect.setHeight(size.y());

   return rect.contains(transformedPoint);
}

QPoint NewChartLabel::getPoint1Transformed(const QwtPlotCurve* curve) const
{
    UTILS_ASSERT(plot() && point1);
    const double x = plot()->transform(curve->xAxis(), point1->getPosition().x() );
    const double y = plot()->transform(curve->yAxis(), point1->getPosition().y() );
    return QPoint(static_cast<int>(x), static_cast<int>(y));
}

QPoint NewChartLabel::getPoint2Transformed(const QwtPlotCurve* curve) const
{
    UTILS_ASSERT(plot() && point2);
    const double x = plot()->transform(curve->xAxis(), point2->getPosition().x() );
    const double y = plot()->transform(curve->yAxis(), point2->getPosition().y() );
    return QPoint(static_cast<int>(x), static_cast<int>(y));
}

void NewChartLabel::connectDot( NewChartDotConstPtr dot, ConnectionStyle style /*= Simple*/ )
{
    point1 = dot;
    point2 = NewChartDotConstPtr();
    connectionStyle = style;
}

void NewChartLabel::connectDots( NewChartDotConstPtr point1, NewChartDotConstPtr point2, ConnectionStyle style )
{
    this->point1 = point1;
    this->point2 = point2;
    this->connectionStyle = style;
}

void NewChartLabel::connectDots( const QPointF& point1, const QPointF& point2, ConnectionStyle style )
{
    NewChartDotPtr p1(new NewChartDot(point1));
    NewChartDotPtr p2(new NewChartDot(point2));
    connectDots(p1, p2, style);
}

void NewChartLabel::drawConnection(QPainter* painter, const QRect& box, const QPoint& transformedTo, ConnectionStyle style, bool arrowOutside) const
{
    switch(style) {
        case Simple: {
            int pX = utils::clamp(transformedTo.x(), box.left(), box.right());
            int pY = utils::clamp(transformedTo.y(), box.top(), box.bottom());
            drawArrow(painter, QPoint(pX, pY), transformedTo, arrowOutside);
            } break;

        case Horizontal: {
            QPoint transformedFrom(box.left(), box.top() + box.height() / 2);
            QPoint p(transformedTo.x(), transformedFrom.y());
            drawArrow(painter, transformedFrom, p, arrowOutside);
            painter->drawLine(transformedTo, p);
            } break;

        case Vertical: {
            QPoint transformedFrom(box.left() + box.width() / 2, box.top());
            QPoint p(transformedFrom.x(), transformedTo.y());
            drawArrow(painter, transformedFrom, p, arrowOutside);
            painter->drawLine(p, transformedTo);
            } break;

        default:
            UTILS_ASSERT(false);
    }
}

void NewChartLabel::drawArrow( QPainter* painter, const QPoint& transformedFrom, const QPoint& transformedTo, bool outside ) const
{
    painter->drawLine(transformedFrom, transformedTo);
    QPointF normal = (transformedTo- transformedFrom);
    float length = sqrt(normal.x() * normal.x() + normal.y() * normal.y());
    if (length < 0.0001f) {
        return;
    }
    normal.setX(normal.x() / length);
    normal.setY(normal.y() / length);

    QPointF normalR(-normal.y(), normal.x());
    QPointF base = outside ? (transformedTo + (7 * normal)) : (transformedTo - (7 * normal));
    QPointF pL = base + normalR * 2;
    QPointF pR = base - normalR * 2;

    QPolygon triangle;
    triangle.clear();
    triangle << pL.toPoint();
    triangle << transformedTo;
    triangle << pR.toPoint();

    painter->drawPolygon(triangle);
}

NewChartDot::NewChartDot(const QPointF& position, int size /*= 5*/) :
    position(position),
    size(size)
{
    pen.setColor(Qt::darkBlue);
    brush.setColor(Qt::blue);
    brush.setStyle(Qt::SolidPattern);
    setZ( 1000 );
}

void NewChartDot::draw( QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRectF &canvasRect ) const
{
    painter->setPen(pen);
    //painter->setBrush(brush);
    QBrush b;
    b.setStyle(Qt::NoBrush);
    painter->setBrush(b);

    QPoint transformed = QwtScaleMap::transform(xMap, yMap, position).toPoint();
    painter->drawEllipse(transformed, size, size);
    
    painter->setPen(QPen(QColor(Qt::black)));
    painter->drawEllipse(transformed, 1, 1);
}
