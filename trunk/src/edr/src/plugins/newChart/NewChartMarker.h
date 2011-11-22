/********************************************************************
	created:	2011/11/22
	created:	22:11:2011   12:30
	filename: 	NewChartMarker.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__NEWCHARTMARKER_H__
#define HEADER_GUARD_NEW_CHART__NEWCHARTMARKER_H__

#include <qwt/qwt_plot_marker.h>

class NewChartMarker : public QwtPlotMarker
{
public:
    explicit NewChartMarker(ScalarChannelReaderInterfaceConstPtr reader);
    explicit NewChartMarker() {}
	virtual ~NewChartMarker() {}

protected:
    virtual void drawLines( QPainter *, const QRectF &, const QPointF & ) const;
    virtual void drawLabel( QPainter *, const QRectF &, const QPointF & ) const;

private:
    ScalarChannelReaderInterfaceConstPtr reader;
};
typedef core::shared_ptr<NewChartMarker> NewChartMarkerPtr;
typedef core::shared_ptr<const NewChartMarker> NewChartMarkerConstPtr;


#endif
