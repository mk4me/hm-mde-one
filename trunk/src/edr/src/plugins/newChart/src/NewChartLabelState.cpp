#include "NewChartPCH.h"
#include "NewChartLabelState.h"
#include "NewChartVisualizer.h"
#include "NewChartLabel.h"
#include "NewChartDot.h"

NewChartLabelState::NewChartLabelState(NewChartVisualizer* visualizer) :
    NewChartState(visualizer)
{
}

void NewChartLabelState::move( const QPoint& pos, const QwtPlotCurve* curve, NewChartLabel* label )
{
    label->setShift(pos - label->getPoint1Transformed(curve));
}

NewChartLabelState::LabelDataConstPtr NewChartLabelState::getLabel( const QPoint& pos, const QwtPlotCurve* curve )
{
    const QwtPlotItemList& itmList = plot->itemList();
    for ( QwtPlotItemIterator it = itmList.begin(); it != itmList.end(); ++it ) {
        NewChartLabel* label = dynamic_cast<NewChartLabel*>(*it);
        if (label && label->getPoint1() && label->isInsideLabel(pos, curve)) {            
            LabelDataConstPtr data = getLabel(label);
            if (data) {
                return data;
            }
        }
    }

    return LabelDataConstPtr();
}

NewChartLabelState::LabelDataConstPtr NewChartLabelState::getLabel( const QPoint& pos )
{
    auto range = visualizer->getSeries();
    for (auto it = range.begin(); it != range.end(); it++) {
        LabelDataConstPtr l = getLabel(pos, (*it)->getCurve());
        if (l) {
            return l;
        }
    }

    return LabelDataConstPtr();
}

NewChartLabelState::LabelDataConstPtr NewChartLabelState::getLabel( const NewChartLabel* label ) const
{
    for (auto it = labels.begin(); it != labels.end(); ++it) {
        if ((*it)->label == label) {
            return *it;
        }
    }

    return LabelDataConstPtr();
}

NewChartLabelState::SeriePointDist NewChartLabelState::getClosestPoint(const QPoint& pos) const
{
    double min = (std::numeric_limits<double>::max)();
    const NewChartSerie* serie = nullptr;
    QPointF ret;
    auto series = visualizer->getSeries();
    for (auto it = series.begin(); it != series.end(); ++it) {
        double dist;
        const QwtPlotCurve* c = (*it)->getCurve();
        int pointIndex = c->closestPoint(pos, &dist);
        if (dist < min) {
            min = dist;
            serie = *it;
            ret = c->sample( pointIndex );
        }
    }
    
    return boost::make_tuple(serie, ret, min);
}

void NewChartLabelState::draw( QPainter * painter)
{
    //throw std::exception("The method or operation is not implemented.");
}

void NewChartLabelState::stateBegin()
{
    canvas->setMouseTracking(true);
    canvas->setCursor(Qt::ForbiddenCursor);
}

void NewChartLabelState::stateEnd()
{
    //marker.detach();
}

void NewChartLabelState::removeSerieLabels( const NewChartSerie* serie )
{
    for (auto it = labels.begin(); it != labels.end(); ++it) {
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
    for (auto it = labels.begin(); it != labels.end(); ++it) {
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


                                         
                                 
