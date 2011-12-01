#include "NewChartPCH.h"
#include "NewChartValueMarker.h"
#include "NewChartSerie.h"
#include "NewChartVisualizer.h"

static const double MIN_DIST = 20.0;

NewChartValueMarker::NewChartValueMarker( NewChartVisualizer* visualizer ) : 
    NewChartLabelState(visualizer),
    currentLabel(nullptr)
{
    marker.setLerpRatios(1.0f);
}

bool NewChartValueMarker::stateEventFilter( QObject *object, QEvent *event )
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
                insertNewMarker(sample, currentSerie->getColor());
            }
        }
    } else if (event->type() == QEvent::MouseButtonRelease) {
        currentLabel = nullptr;
    } else if (event->type() == QEvent::MouseMove) {
        QMouseEvent* mouseEvent = (QMouseEvent*)event;
        
        if (currentLabel) {
            marker.setVisible(false);
            move(mouseEvent->pos(), currentSerie->getCurve(), currentLabel);
            currentLabel->itemChanged();
        } else {
            currentLabel = nullptr;
            QPointF sample;
            double dist = getClosestPoint(sample, currentSerie->getCurve(), mouseEvent->pos());
            if (dist < MIN_DIST) {
                marker.setValue(sample);
                marker.setVisible(true);
            } else {
                marker.setVisible(false);
            }
        }
    }

    return QObject::eventFilter( object, event );
}

void NewChartValueMarker::stateBegin()
{
    NewChartLabelState::stateBegin();
    canvas->setCursor(Qt::ForbiddenCursor);
    visualizer->setManipulation(false);
    marker.attach(plot);
}

void NewChartValueMarker::stateEnd()
{
    marker.detach();
}

void NewChartValueMarker::insertNewMarker( const QPointF& point, const QColor& color )
{
    NewChartDotPtr dot(new NewChartDot(point, 5));
    NewChartLabelPtr label(new NewChartLabel(QString("Time: %1\nValue: %2").arg(point.x()).arg(point.y())));
    label->setPen(QPen(color));
    dot->attach(plot);
    label->attach(plot);
    label->connectDot(dot);
    labels.push_back(label);
}



