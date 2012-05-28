/********************************************************************
	created:	2011/11/25
	created:	25:11:2011   8:50
	filename: 	NewChartState.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__NEWCHARTSTATE_H__
#define HEADER_GUARD_NEW_CHART__NEWCHARTSTATE_H__

#include <QtCore/QObject>

class NewChartVisualizer;
class QwtPlot;
class QEvent;
class QPainter;
class QwtPlotCanvas;

class NewChartState : public QObject
{
    Q_OBJECT
public:
    typedef core::shared_ptr<QwtPlot> QwtPlotPtr;

public:
    NewChartState(NewChartVisualizer* visualizer);
	virtual ~NewChartState() { }

public:
    virtual bool stateEventFilter( QObject *, QEvent * ) = 0;
    virtual void draw(QPainter *) = 0;
    virtual void stateBegin() {}
    virtual void stateEnd() {}

protected:
    NewChartVisualizer* visualizer;
    QwtPlot* plot;
    QwtPlotCanvas* canvas;
};
typedef core::shared_ptr<NewChartState> NewChartStatePtr;
typedef core::shared_ptr<const NewChartState> NewChartStateConstPtr;

#endif
