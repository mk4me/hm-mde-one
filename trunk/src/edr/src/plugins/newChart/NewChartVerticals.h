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

//! Stan wizualizatora odpowiadaj¹cy za etykiety roznicowe
class NewChartVerticals : public NewChartLabelState
{
public:
    //! Konstruktor
    //! \param visualizer wizualizator, na którym stan bêdzie operowa³
    //! \param style styl po³¹czeñ miêdzy wskazywanymi przez etykietê punktami
    NewChartVerticals(NewChartVisualizer* visualizer, NewChartLabel::ConnectionStyle style);
    virtual ~NewChartVerticals() {}

public:
    //! Filtrowanie eventów Qt (g³ównie myszka) zarz¹dzanie etykietami
    virtual bool stateEventFilter(QObject *, QEvent *);
    //! rozpoczyna stan, zmienia kursor, podpina pod wykres dodatkowy wskaŸnik (u³atwiaj¹cy dodawanie nowych etykiet)
    virtual void stateBegin();
    //! koñczy stan, usuwa dodatkowy wskaŸnik z wykresu
    virtual void stateEnd();

private:
    //! Wstawienie nowej etykiety
    //! \param point1 pierwszy wskazywany punkt
    //! \param point2 drugi wskazywany punkt
    //! \param currentSerie seria, z która jest zwiazana etykieta
    //! \param color kolor obwódki etykiety
    void insertNewMarker(const QPointF& point1, const QPointF& point2, const NewChartSerie* currentSerie, const QColor& color = QColor(Qt::blue));
    //! odœwie¿enie etykiet
    void updateLabels();

private:
    //! dodatkowy wskaŸnik (u³atwiaj¹cy dodawanie nowych etykiet)
    NewChartLabelPtr labelMarker;
    //! etykieta, która jest obecnie przesuwana (mo¿e to byæ dowolna etykieta)
    LabelDataConstPtr currentLabel;
    //! wybrany, pierwszy punkt
    boost::shared_ptr<std::pair<const NewChartSerie*, QPointF>> point1;
    //! styl po³¹czenia (pionowe, poziome, proste)
    NewChartLabel::ConnectionStyle style;
    //! marker obrazuj¹cy pierwszy wskazywany punkt 
    NewChartMarker marker;
};
typedef core::shared_ptr<NewChartVerticals> NewChartVerticalsPtr;
typedef core::shared_ptr<const NewChartVerticals> NewChartVerticalsConstPtr;


#endif
