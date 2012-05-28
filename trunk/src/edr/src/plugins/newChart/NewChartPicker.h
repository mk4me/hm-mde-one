
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

//! klasa reprezentuje stan wizualizatora pozwalajacy wybor aktywnej serii danych
class NewChartPicker: public NewChartState
{
    Q_OBJECT
public:
    //! Konstruktor pobiera wizualizator, na ktorym stan bedzie operowal
    NewChartPicker( NewChartVisualizer* visualizer );
	virtual ~NewChartPicker() {}

public:
    //! filtrowanie eventow Qt, obsluga myszki
    virtual bool stateEventFilter( QObject *, QEvent * );
    //! picker nie musi sie odrysowywac
    virtual void draw(QPainter* painter) {} 
    //! rozpoczecie stanu, zmiana kursora
    virtual void stateBegin();
    //! nic nie trzeba robic po zakonczeniu stanu
    virtual void stateEnd() {}
    //! \return tolerancja w pikselach wg ktorej zaliczane jest klikniecie na krzywa
    int getPixelTolerance() const { return pixelTolerance; }
    //! ustawia tolerancje wg ktorej zaliczane jest klikniecie na krzywa
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
    //! metoda obsluguje klikniecie w wykres, sprawdza, czy kliknieto w krzywa i ewentualnie ja zaznacza
    //! \param pos kliknieta pozycja
    bool select( const QPoint & pos);

private:
    //! plotno wykresu
    QwtPlotCanvas* canvas;
    //! tolerancja, czyli maksymalna odlegnosc od krzywej zaliczajaca klikniecie w nia
    int pixelTolerance;
    //! aktualnie wybrana krzywa, klikniecie w nia bedzie ignorowane
    QwtPlotCurve* currentCurve;
    //! flaga okreslajaca, czy kursor sie porusza, jesli tak, to nie mozna wykonac wszystkich akcji pickera
    bool move;
};
typedef core::shared_ptr<NewChartPicker> NewChartPickerPtr;
typedef core::shared_ptr<const NewChartPicker> NewChartPickerConstPtr;

#endif
