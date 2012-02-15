/********************************************************************
	created:	2011/11/28
	created:	28:11:2011   22:37
	filename: 	NewChartLabelState.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__NEWCHARTLABELSTATE_H__
#define HEADER_GUARD_NEW_CHART__NEWCHARTLABELSTATE_H__

#include "NewChartMarker.h"
#include "NewChartState.h"

class NewChartSerie;
class NewChartLabelState : public NewChartState
{
public:
    NewChartLabelState(NewChartVisualizer* visualizer);
	virtual ~NewChartLabelState() {}

public:
    virtual void draw( QPainter * );
    virtual void stateBegin();
    virtual void stateEnd();

    void removeSerieLabels(const NewChartSerie* serie);
    void setVisible(const NewChartSerie* serie, bool visible);

protected:
    void move(const QPoint& pos, const QwtPlotCurve* curve, NewChartLabel* label);
    NewChartLabel* getLabel(const QPoint& pos, const QwtPlotCurve* curve);
    double getClosestPoint(QPointF& ret, const QwtPlotCurve* curve, const QPoint& pos);

protected:
    struct LabelData
    {
        NewChartLabelPtr label;
        NewChartDotPtr dot1;
        NewChartDotPtr dot2;
        const NewChartSerie* serie;
    };
    typedef core::shared_ptr<LabelData> LabelDataPtr;
    typedef core::shared_ptr<const LabelData> LabelDataConstPtr;

protected:
    std::vector<LabelDataPtr> labels;

};
typedef core::shared_ptr<NewChartLabelState> NewChartLabelStatePtr;
typedef core::shared_ptr<const NewChartLabelState> NewChartLabelStateConstPtr;


#endif
