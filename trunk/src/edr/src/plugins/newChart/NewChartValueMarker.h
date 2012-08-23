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
    //! \param visualizer wizualizator, na którym stan bêdzie operowa³
    NewChartValueMarker(NewChartVisualizer* visualizer);
    virtual ~NewChartValueMarker() {}

public:
    //! Filtrowanie eventów Qt (g³ównie myszka) zarz¹dzanie etykietami
    virtual bool stateEventFilter(QObject *, QEvent *);
    //! rozpoczyna stan, zmienia kursor, podpina pod wykres dodatkowy wskaŸnik (u³atwiaj¹cy dodawanie nowych etykiet)
    virtual void stateBegin();
    //! koñczy stan, usuwa dodatkowy wskaŸnik z wykresu
    virtual void stateEnd();

private:
    //! Wstawienie nowej etykiety
    //! \param point wskazywany punkt
    //! \param color kolor obwodki etykiety
    void insertNewMarker(const QPointF& point, const QColor& color = QColor(Qt::blue));
    //! odœwie¿enie etykiet
    void updateLabels();

private:
    //! dodatkowy wskaŸnik (u³atwiaj¹cy dodawanie nowych etykiet)
    NewChartMarker marker;
    //! etykieta, która jest obecnie przesuwana (mo¿e to byæ dowolna etykieta)
    LabelDataConstPtr currentLabel;
};
typedef core::shared_ptr<NewChartValueMarker> NewChartValueMarkerPtr;
typedef core::shared_ptr<const NewChartValueMarker> NewChartValueMarkerConstPtr;

#endif
