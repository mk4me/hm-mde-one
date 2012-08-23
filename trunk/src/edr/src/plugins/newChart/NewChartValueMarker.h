/********************************************************************
	created:	2011/11/25
	created:	25:11:2011   8:54
	filename: 	NewChartValueMarker.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__NEWCHARTVALUEMARKER_H__
#define HEADER_GUARD_NEW_CHART__NEWCHARTVALUEMARKER_H__

#include "NewChartState.h"
#include "NewChartMarker.h"
#include "NewChartLabelState.h"

//! stan wizualizatora za etykiety z wartosciami
class NewChartValueMarker : public NewChartLabelState
{
public:
    //! Konstruktor
    //! \param visualizer wizualizator, na kt�rym stan b�dzie operowa�
    NewChartValueMarker(NewChartVisualizer* visualizer);
    virtual ~NewChartValueMarker() {}

public:
    //! Filtrowanie event�w Qt (g��wnie myszka) zarz�dzanie etykietami
    virtual bool stateEventFilter(QObject *, QEvent *);
    //! rozpoczyna stan, zmienia kursor, podpina pod wykres dodatkowy wska�nik (u�atwiaj�cy dodawanie nowych etykiet)
    virtual void stateBegin();
    //! ko�czy stan, usuwa dodatkowy wska�nik z wykresu
    virtual void stateEnd();

private:
    //! Wstawienie nowej etykiety
    //! \param point wskazywany punkt
    //! \param color kolor obwodki etykiety
    void insertNewMarker(const QPointF& point, const QColor& color = QColor(Qt::blue));
    //! od�wie�enie etykiet
    void updateLabels();

private:
    //! dodatkowy wska�nik (u�atwiaj�cy dodawanie nowych etykiet)
    NewChartMarker marker;
    //! etykieta, kt�ra jest obecnie przesuwana (mo�e to by� dowolna etykieta)
    LabelDataConstPtr currentLabel;
};
typedef core::shared_ptr<NewChartValueMarker> NewChartValueMarkerPtr;
typedef core::shared_ptr<const NewChartValueMarker> NewChartValueMarkerConstPtr;

#endif
