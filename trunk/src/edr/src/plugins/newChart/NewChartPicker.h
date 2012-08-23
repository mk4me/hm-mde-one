
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

//! klasa reprezentuje stan wizualizatora pozwalaj¹cy wybor aktywnej serii danych
class NewChartPicker: public NewChartState
{
    Q_OBJECT
public:
    //! Konstruktor pobiera wizualizator, na którym stan bêdzie operowa³
    NewChartPicker( NewChartVisualizer* visualizer );
	virtual ~NewChartPicker() {}

public:
    //! filtrowanie eventów Qt, obs³uga myszki
    virtual bool stateEventFilter( QObject *, QEvent * );
    //! picker nie musi siê odrysowywaæ
    virtual void draw(QPainter* painter) {} 
    //! rozpoczêcie stanu, zmiana kursora
    virtual void stateBegin();
    //! nic nie trzeba robic po zakoñczeniu stanu
    virtual void stateEnd() {}
    //! \return tolerancja w pikselach wg której zaliczane jest klikniecie na krzywa
    int getPixelTolerance() const { return pixelTolerance; }
    //! ustawia tolerancje wg której zaliczane jest klikniecie na krzywa
    //! \param val nowa tolerancja w pikselach
    void setPixelTolerance(int val) { pixelTolerance = val; }
    //! ustawia aktualnie aktywna krzywa, klikniecia na nia beda ignorowane
    //! \param curve aktualnie aktywna krzywa
    void setCurrentCurve(QwtPlotCurve * curve);

signals:
    //! sygnal emitowany, gdy kliknieta zostanie krzywa
    //! \param curve kliknieta krzywa
    void serieSelected(QwtPlotItem* curve);

private:
    //! metoda obs³uguje klikniêcie w wykres, sprawdza, czy klikniêto w krzyw¹ i ewentualnie ja zaznacza
    //! \param pos kliknieta pozycja
    bool select( const QPoint & pos);

private:
    //! plotno wykresu
    QwtPlotCanvas* canvas;
    //! tolerancja, czyli maksymalna odleg³oœæ od krzywej zaliczaj¹ca klikniêcie w ni¹
    int pixelTolerance;
    //! aktualnie wybrana krzywa, klikniêcie w ni¹ bêdzie ignorowane
    QwtPlotCurve* currentCurve;
    //! flaga okreœlaj¹ca, czy kursor siê porusza, jeœli tak, to nie mo¿na wykonaæ wszystkich akcji pickera
    bool move;
};
typedef core::shared_ptr<NewChartPicker> NewChartPickerPtr;
typedef core::shared_ptr<const NewChartPicker> NewChartPickerConstPtr;

#endif
