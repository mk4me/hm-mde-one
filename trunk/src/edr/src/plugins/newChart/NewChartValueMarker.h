/********************************************************************
	created:	2011/11/25
	created:	25:11:2011   8:54
	filename: 	NewChartValueMarker.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__NEWCHARTVALUEMARKER_H__
#define HEADER_GUARD_NEW_CHART__NEWCHARTVALUEMARKER_H__

#include "NewChartState.h"
#include "NewChartMarker.h"
#include "NewChartLabelState.h"
#include <qwt/qwt_plot_curve.h>

class NewChartValueMarker : public NewChartLabelState
{
public:
    NewChartValueMarker(NewChartVisualizer* visualizer);
    virtual ~NewChartValueMarker() {}

public:
    virtual bool stateEventFilter(QObject *, QEvent *);
    virtual void stateBegin();
    virtual void stateEnd();

private:
    void insertNewMarker(const QPointF& point, const QColor& color = QColor(Qt::blue));
    void updateLabels();

private:
    NewChartMarker marker;
    LabelDataConstPtr currentLabel;
};
typedef core::shared_ptr<NewChartValueMarker> NewChartValueMarkerPtr;
typedef core::shared_ptr<const NewChartValueMarker> NewChartValueMarkerConstPtr;

#endif
