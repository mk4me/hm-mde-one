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
    marker.setLerpRatios(1.0f);
}

bool NewChartVerticals::stateEventFilter( QObject *object, QEvent *event )
{
    const NewChartSerie* currentSerie = visualizer->tryGetCurrentSerie();
    if (object != canvas || !currentSerie ) {
        return false;
    }
    
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
            
            if (point1) {
                insertNewMarker(*point1, sample, currentSerie->getColor());
                point1.reset();
            } else {
                if (dist < MIN_DIST) {
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
            if (!point1 && dist < MIN_DIST) {
                marker.setVisible(true);
                marker.setValue(sample);
            } else {  
                marker.setVisible(false);
            }
            if ( point1) {
                QString serieName = (style == NewChartLabel::Horizontal) ? "t" : currentSerie->getName().c_str();
                double diff = (style == NewChartLabel::Horizontal) ? (sample.x() - point1->x()) : (sample.y() - point1->y());
                labelMarker->setText(QString("%1%2: %3").arg(QChar(0x394)).arg(serieName).arg(diff));
                labelMarker->connectDots(sample, *point1, style);
                labelMarker->setVisible(true);
                labelMarker->itemChanged();
            } else {
                labelMarker->setVisible(false);
            }
        }
    } else  if (event->type() == QEvent::Paint) {
        updateLabels();
    }

    return QObject::eventFilter( object, event );
}

void NewChartVerticals::stateBegin()
{
    NewChartLabelState::stateBegin();
    visualizer->setManipulation(false);
    canvas->setCursor(Qt::DragLinkCursor);
    labelMarker->attach(plot);
    marker.attach(plot);
}

void NewChartVerticals::stateEnd()
{
    labelMarker->detach();
    marker.detach();
}

void NewChartVerticals::insertNewMarker( const QPointF& point1, const QPointF& point2, const QColor& color )
{
    const NewChartSerie* currentSerie = visualizer->tryGetCurrentSerie();
    UTILS_ASSERT(currentSerie);
    NewChartDotPtr dot1(new NewChartDotFloating(point1, currentSerie));
    NewChartDotPtr dot2(new NewChartDotFloating(point2, currentSerie));
    double delta = (style == NewChartLabel::Vertical) ? (point2.y() - point1.y()) : (point2.x() - point1.x());
    QString serieName = (style == NewChartLabel::Horizontal) ? "t" : currentSerie->getName().c_str();
    NewChartLabelPtr label(new NewChartLabel(QString("%1%2: %3").arg(QChar(0x394)).arg(serieName).arg(delta)));
    label->setPen(QPen(color));
    dot1->attach(plot);
    dot2->attach(plot);
    label->attach(plot);
    label->connectDots(dot2, dot1, style);

    LabelDataPtr data(new LabelData);
    data->dot1 = dot1;
    data->dot2 = dot2;
    data->label = label;
    data->serie = currentSerie;
    labels.push_back(data);
}

void NewChartVerticals::updateLabels()
{
    for (auto it = labels.begin(); it != labels.end(); it++) {
        QPointF p1 = (*it)->dot1->getPosition();
        QPointF p2 = (*it)->dot2->getPosition();
        double delta = (style == NewChartLabel::Vertical) ? (p2.y() - p1.y()) : (p2.x() - p1.x());
        QString serieName = (style == NewChartLabel::Horizontal) ? "t" : (*it)->serie->getName().c_str();
        (*it)->label->setText(QString("%1%2: %3").arg(QChar(0x394)).arg(serieName).arg(delta));
    }
}

