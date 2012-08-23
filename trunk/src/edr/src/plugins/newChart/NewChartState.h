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
#include <core/SmartPtr.h>

class NewChartVisualizer;
class QwtPlot;
class QEvent;
class QPainter;
class QwtPlotCanvas;

//! Klasa bazowa dla stanow wizualizatora
class NewChartState : public QObject
{
    Q_OBJECT
public:
    typedef core::shared_ptr<QwtPlot> QwtPlotPtr;

public:
    //! kostruktor pobiera obiekt wizualizatora, którym bêdzie siê zarz¹dzalo
    NewChartState(NewChartVisualizer* visualizer);
	virtual ~NewChartState() { }

public:
    //! Obsluga eventow Qt
    virtual bool stateEventFilter( QObject *, QEvent * ) = 0;
    //! Metoda odrysowuj¹ca stan wizualizatora
    virtual void draw(QPainter *) = 0;
    //! wywo³ywane, stan staje siê aktywny
    virtual void stateBegin() {}
    //! wywo³ywane, gdy stan przestaje byæ aktywny
    virtual void stateEnd() {}

protected:
    //! wizualizator, którym bêdzie siê zarz¹dzalo
    NewChartVisualizer* visualizer;
    //! Wykres qwt (serce wizualizatora)
    QwtPlot* plot;
    //! Plotno wykresu
    QwtPlotCanvas* canvas;
};
typedef core::shared_ptr<NewChartState> NewChartStatePtr;
typedef core::shared_ptr<const NewChartState> NewChartStateConstPtr;

#endif
