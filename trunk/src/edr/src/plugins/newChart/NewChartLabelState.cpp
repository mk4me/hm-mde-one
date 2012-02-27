#include "NewChartPCH.h"
#include "NewChartLabelState.h"
#include "NewChartVisualizer.h"

NewChartLabelState::NewChartLabelState(NewChartVisualizer* visualizer) :
    NewChartState(visualizer)
{

}

void NewChartLabelState::move( const QPoint& pos, const QwtPlotCurve* curve, NewChartLabel* label )
{
    label->setShift(pos - label->getPoint1Transformed(curve));
}

NewChartLabel* NewChartLabelState::getLabel( const QPoint& pos, const QwtPlotCurve* curve )
{
    const QwtPlotItemList& itmList = plot->itemList();
    for ( QwtPlotItemIterator it = itmList.begin(); it != itmList.end(); ++it ) {
        NewChartLabel* label = dynamic_cast<NewChartLabel*>(*it);
        if (label && label->getPoint1() && label->isInsideLabel(pos, curve)) {            
            return label;
        }
    }

    return nullptr;
}

double NewChartLabelState::getClosestPoint( QPointF& ret, const QwtPlotCurve* curve, const QPoint& pos )
{
    UTILS_ASSERT(curve);

    double d = 0.0;
    int pointIndex = curve->closestPoint(pos, &d);
    ret = curve->sample( pointIndex );
    return d;
}

void NewChartLabelState::draw( QPainter * painter)
{
    //throw std::exception("The method or operation is not implemented.");
}

void NewChartLabelState::stateBegin()
{
    canvas->setMouseTracking(true);
    canvas->setCursor(Qt::ForbiddenCursor);
    //marker.attach(plot);
    
}

void NewChartLabelState::stateEnd()
{
    //marker.detach();
}

void NewChartLabelState::removeSerieLabels( const NewChartSerie* serie )
{
    for (auto it = labels.begin(); it != labels.end(); it++) {
        LabelDataPtr data = *it;
        if (data->serie == serie) {
            if (data->dot1) {
                data->dot1->detach();
            }
            if (data->dot2) {
                data->dot2->detach();
            }
            if (data->label) {
                data->label->detach();
            }
        }
    }
}

void NewChartLabelState::setVisible( const NewChartSerie* serie, bool visible )
{
    for (auto it = labels.begin(); it != labels.end(); it++) {
        LabelDataPtr data = *it;
        if (data->serie == serie) {
            if (data->dot1) {
                data->dot1->setVisible(visible);
            }
            if (data->dot2) {
                data->dot2->setVisible(visible);
            }
            if (data->label) {
                data->label->setVisible(visible);
            }
        }
    }
}


                                         
                                 