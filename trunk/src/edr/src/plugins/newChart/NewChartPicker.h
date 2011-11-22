
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

class NewChartPicker: public QObject
{
    Q_OBJECT
public:
    NewChartPicker( QwtPlot* plot );
	virtual ~NewChartPicker() {}

public:
    virtual bool eventFilter( QObject *, QEvent * );
    //virtual bool event( QEvent * );

signals:
    void serieSelected(QwtPlotItem* curve);

private:
    void select( const QPoint & );
    /*void move( const QPoint & );
    void moveBy( int dx, int dy );*/

    //void release();

    /*void showCursor( bool enable );
    void shiftPointCursor( bool up );
    void shiftCurveCursor( bool up );*/

    /*QwtPlot *plot();
    const QwtPlot *plot() const;

    QwtPlotCurve *d_selectedCurve;
    int d_selectedPoint;*/

private:
    QwtPlot* plot;
};
typedef core::shared_ptr<NewChartPicker> NewChartPickerPtr;
typedef core::shared_ptr<const NewChartPicker> NewChartPickerConstPtr;

#endif
