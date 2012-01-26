
/********************************************************************
	created:	2011/11/18
	created:	18:11:2011   11:21
	filename: 	NewChartPicker.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART_VISUALIZER__NEWCHARTPICKER_H__
#define HEADER_GUARD_NEW_CHART_VISUALIZER__NEWCHARTPICKER_H__

#include <QtCore/QObject>

class QPoint;
class QCustomEvent;
class QwtPlot;
class QwtPlotItem;

#include "NewChartSerie.h"
#include "NewChartState.h"

class NewChartPicker: public NewChartState
{
    Q_OBJECT
public:
    NewChartPicker( NewChartVisualizer* visualizer );
	virtual ~NewChartPicker() {}

public:
    virtual bool stateEventFilter( QObject *, QEvent * );
    virtual void draw(QPainter* painter) {} 
    virtual void stateBegin();
    virtual void stateEnd() {}

    int getPixelTolerance() const { return pixelTolerance; }
    void setPixelTolerance(int val) { pixelTolerance = val; }
    void setCurrentCurve(QwtPlotCurve * curve);

signals:
    void serieSelected(QwtPlotItem* curve);

private:
    bool select( const QPoint & );

private:
    QwtPlotCanvas* canvas;
    int pixelTolerance;
    QwtPlotCurve* currentCurve;
    bool move;
};
typedef core::shared_ptr<NewChartPicker> NewChartPickerPtr;
typedef core::shared_ptr<const NewChartPicker> NewChartPickerConstPtr;

#endif
