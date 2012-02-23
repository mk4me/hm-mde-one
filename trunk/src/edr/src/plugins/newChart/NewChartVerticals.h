/********************************************************************
	created:	2011/11/25
	created:	25:11:2011   8:56
	filename: 	NewChartVerticals.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__NEWCHARTVERTICALS_H__
#define HEADER_GUARD_NEW_CHART__NEWCHARTVERTICALS_H__

#include <qwt/qwt_plot_curve.h>
#include "NewChartState.h"
#include "NewChartMarker.h"
#include "NewChartLabelState.h"


class NewChartVerticals : public NewChartLabelState
{
public:
    NewChartVerticals(NewChartVisualizer* visualizer, NewChartLabel::ConnectionStyle style);
    virtual ~NewChartVerticals() {}

public:
    virtual bool stateEventFilter( QObject *, QEvent * );
    virtual void stateBegin();
    virtual void stateEnd();

private:
    void insertNewMarker(const QPointF& point1, const QPointF& point2, const QColor& color = QColor(Qt::blue));
    void updateLabels();

private:
    NewChartLabelPtr labelMarker;
    NewChartLabel* currentLabel;
    boost::shared_ptr<QPointF> point1;
    NewChartLabel::ConnectionStyle style;
    NewChartMarker marker;
};
typedef core::shared_ptr<NewChartVerticals> NewChartVerticalsPtr;
typedef core::shared_ptr<const NewChartVerticals> NewChartVerticalsConstPtr;


#endif
