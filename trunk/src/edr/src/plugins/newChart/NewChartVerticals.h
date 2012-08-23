/********************************************************************
	created:	2011/11/25
	created:	25:11:2011   8:56
	filename: 	NewChartVerticals.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__NEWCHARTVERTICALS_H__
#define HEADER_GUARD_NEW_CHART__NEWCHARTVERTICALS_H__

//#include <qwt/qwt_plot_curve.h>
#include "NewChartState.h"
#include "NewChartLabel.h"
#include "NewChartMarker.h"
#include "NewChartLabelState.h"

//! Stan wizualizatora odpowiadajacy za etykiety roznicowe
class NewChartVerticals : public NewChartLabelState
{
public:
    //! Konstruktor
    //! \param visualizer wizualizator, na kt�rym stan b�dzie operowa�
    //! \param style styl po��cze� mi�dzy wskazywanymi przez etykiete punktami
    NewChartVerticals(NewChartVisualizer* visualizer, NewChartLabel::ConnectionStyle style);
    virtual ~NewChartVerticals() {}

public:
    //! Filtrowanie eventow Qt (g��wnie myszka) zarz�dzanie etykietami
    virtual bool stateEventFilter(QObject *, QEvent *);
    //! rozpoczyna stan, zmienia kursor, podpina pod wykres dodatkowy wska�nik (ulatwiajacy dodawanie nowych etykiet)
    virtual void stateBegin();
    //! konczy stan, usuwa dodatkowy wska�nik z wykresu
    virtual void stateEnd();

private:
    //! Wstawienie nowej etykiety
    //! \param point1 pierwszy wskazywany punkt
    //! \param point2 drugi wskazywany punkt
    //! \param currentSerie seria, z kt�ra jest zwiazana etykieta
    //! \param color kolor obwodki etykiety
    void insertNewMarker(const QPointF& point1, const QPointF& point2, const NewChartSerie* currentSerie, const QColor& color = QColor(Qt::blue));
    //! od�wie�enie etykiet
    void updateLabels();

private:
    //! dodatkowy wska�nik (ulatwiajacy dodawanie nowych etykiet)
    NewChartLabelPtr labelMarker;
    //! etykieta, kt�ra jest obecnie przesuwana (mo�e to by� dowolna etykieta)
    LabelDataConstPtr currentLabel;
    //! wybrany, pierwszy punkt
    boost::shared_ptr<std::pair<const NewChartSerie*, QPointF>> point1;
    //! styl po��czenia (pionowe, poziome, proste)
    NewChartLabel::ConnectionStyle style;
    //! marker obrazujacy pierwszy wskazywany punkt 
    NewChartMarker marker;
};
typedef core::shared_ptr<NewChartVerticals> NewChartVerticalsPtr;
typedef core::shared_ptr<const NewChartVerticals> NewChartVerticalsConstPtr;


#endif
