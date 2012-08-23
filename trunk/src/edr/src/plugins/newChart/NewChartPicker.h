
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

//! klasa reprezentuje stan wizualizatora pozwalaj�cy wybor aktywnej serii danych
class NewChartPicker: public NewChartState
{
    Q_OBJECT
public:
    //! Konstruktor pobiera wizualizator, na kt�rym stan b�dzie operowa�
    NewChartPicker( NewChartVisualizer* visualizer );
	virtual ~NewChartPicker() {}

public:
    //! filtrowanie event�w Qt, obs�uga myszki
    virtual bool stateEventFilter( QObject *, QEvent * );
    //! picker nie musi si� odrysowywa�
    virtual void draw(QPainter* painter) {} 
    //! rozpocz�cie stanu, zmiana kursora
    virtual void stateBegin();
    //! nic nie trzeba robic po zako�czeniu stanu
    virtual void stateEnd() {}
    //! \return tolerancja w pikselach wg kt�rej zaliczane jest klikniecie na krzywa
    int getPixelTolerance() const { return pixelTolerance; }
    //! ustawia tolerancje wg kt�rej zaliczane jest klikniecie na krzywa
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
    //! metoda obs�uguje klikni�cie w wykres, sprawdza, czy klikni�to w krzyw� i ewentualnie ja zaznacza
    //! \param pos kliknieta pozycja
    bool select( const QPoint & pos);

private:
    //! plotno wykresu
    QwtPlotCanvas* canvas;
    //! tolerancja, czyli maksymalna odleg�o�� od krzywej zaliczaj�ca klikni�cie w ni�
    int pixelTolerance;
    //! aktualnie wybrana krzywa, klikni�cie w ni� b�dzie ignorowane
    QwtPlotCurve* currentCurve;
    //! flaga okre�laj�ca, czy kursor si� porusza, je�li tak, to nie mo�na wykona� wszystkich akcji pickera
    bool move;
};
typedef core::shared_ptr<NewChartPicker> NewChartPickerPtr;
typedef core::shared_ptr<const NewChartPicker> NewChartPickerConstPtr;

#endif
