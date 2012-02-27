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
    } else if (event->type() == QEvent::Paint) {
        updateLabels();
        
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
    const NewChartSerie* serie = visualizer->tryGetCurrentSerie();
    NewChartDotPtr dot(new NewChartDotFloating(point, serie));
    NewChartLabelPtr label(new NewChartLabel(QObject::tr("Time: %1\nValue: %2").arg(point.x()).arg(point.y())));
    label->setPen(QPen(color));
    dot->attach(plot);
    label->attach(plot);
    label->connectDot(dot);

    LabelDataPtr data(new LabelData());
    data->dot1 = dot;
    data->label = label;
    data->serie = serie;

    labels.push_back(data);
}

void NewChartValueMarker::updateLabels()
{
    for (auto it = labels.begin(); it != labels.end(); it++) {
        const QPointF& p = (*it)->dot1->getPosition();
        (*it)->label->setText(QObject::tr("Time: %1\nValue: %2").arg(p.x()).arg(p.y()));
    }
}



