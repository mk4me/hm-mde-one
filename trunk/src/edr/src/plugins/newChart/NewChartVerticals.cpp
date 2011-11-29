#include "NewChartPCH.h"
#include "NewChartVerticals.h"
#include "NewChartSerie.h"
#include "NewChartVisualizer.h"

static const double MIN_DIST = 20.0;

NewChartVerticals::NewChartVerticals( NewChartVisualizer* visualizer, NewChartLabel::ConnectionStyle style ) : 
    NewChartLabelState(visualizer),
    currentLabel(nullptr),
    labelMarker(new NewChartLabel()),
    style(style)
{
    UTILS_ASSERT(style == NewChartLabel::Horizontal || style == NewChartLabel::Vertical);
    labelMarker->attach(plot);
    labelMarker->setVisible(false);
}

bool NewChartVerticals::stateEventFilter( QObject *object, QEvent *event )
{
    const NewChartSerie* currentSerie = visualizer->tryGetCurrentSerie();
    if (object != canvas || !currentSerie ) {
        return false;
    }

    QEvent::Type eventType = event->type();

    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent* mouseEvent = (QMouseEvent*)event;
        if (mouseEvent->button() == Qt::LeftButton) {
            currentLabel = getLabel(mouseEvent->pos(), currentSerie->getCurve());
        } else {
            currentLabel = nullptr;
        }

        if (!currentLabel && mouseEvent->button() == Qt::LeftButton) {
            QPointF sample;
            double dist = getClosestPoint(sample, currentSerie->getCurve(), mouseEvent->pos());
            if (dist < MIN_DIST) {
                if (point1) {
                    insertNewMarker(*point1, sample, currentSerie->getColor());
                    point1.reset();
                } else {
                    point1.reset(new QPointF(sample));
                }
                
            }
        }
    } else if (event->type() == QEvent::MouseButtonRelease) {
        currentLabel = nullptr;
    } else if (event->type() == QEvent::MouseMove) {
        QMouseEvent* mouseEvent = (QMouseEvent*)event;

        if (currentLabel) {
            labelMarker->setVisible(false);
            move(mouseEvent->pos(), currentSerie->getCurve(), currentLabel);
            currentLabel->itemChanged();
        } else {
            currentLabel = nullptr;
            QPointF sample;
            double dist = getClosestPoint(sample, currentSerie->getCurve(), mouseEvent->pos());
            if (dist < MIN_DIST && point1) {
                double diff = (style == NewChartLabel::Horizontal) ? (sample.x() - point1->x()) : (sample.y() - point1->y());
                labelMarker->setText(QString("Difference : %1").arg(diff));
                labelMarker->connectDots(sample, *point1, style);
                labelMarker->setVisible(true);
                labelMarker->itemChanged();
            } else {
                labelMarker->setVisible(false);
            }
        }
    }

    return QObject::eventFilter( object, event );
}

void NewChartVerticals::stateBegin()
{
    NewChartLabelState::stateBegin();
    visualizer->setManipulation(false);
    canvas->setCursor(Qt::DragLinkCursor);
    labelMarker->attach(plot);
}

void NewChartVerticals::stateEnd()
{
    labelMarker->detach();
}

void NewChartVerticals::insertNewMarker( const QPointF& point1, const QPointF& point2, const QColor& color )
{
    NewChartDotPtr dot1(new NewChartDot(point1, 5));
    NewChartDotPtr dot2(new NewChartDot(point2, 5));
    NewChartLabelPtr label(new NewChartLabel(QString("Diff: %1").arg(point2.y() - point1.y())));
    label->setPen(QPen(color));
    dot1->attach(plot);
    dot2->attach(plot);
    label->attach(plot);
    label->connectDots(dot1, dot2, NewChartLabel::Horizontal);
    labels.push_back(label);
}

