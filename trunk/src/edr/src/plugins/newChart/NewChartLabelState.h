/********************************************************************
	created:	2011/11/28
	created:	28:11:2011   22:37
	filename: 	NewChartLabelState.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__NEWCHARTLABELSTATE_H__
#define HEADER_GUARD_NEW_CHART__NEWCHARTLABELSTATE_H__

#include <boost/tuple/tuple.hpp>
#include "NewChartState.h"

class NewChartSerie;
class NewChartDot;
class NewChartLabel;

//! Stan wykresu, który operuje w jakis sposób na etykietach
class NewChartLabelState : public NewChartState
{
public:
    //! Konstruktor pobiera wizualizator, na którym stan bêdzie operowa³
    NewChartLabelState(NewChartVisualizer* visualizer);
	virtual ~NewChartLabelState() {}

public:
    //! w tej klasie metoda jeszcze nie jest wykorzystywana
    virtual void draw(QPainter *);
    //! ustawienie stanu, zmiana kursora
    virtual void stateBegin();
    //! zakoñczenie stanu, nieobs³ugiwane
    virtual void stateEnd();
    //! usuwa etykiety z wykresu zwi¹zane z konkretna seria
    //! \param serie seria, dla której usuwamy
    void removeSerieLabels(const NewChartSerie* serie);
    //! zarz¹dza widocznoœci¹ etykiet z wykresu zwiazanych z konkretna seria
    //! \param serie seria, dla której zmieniamy widocznoœæ elementów
    //! \param visible widoczny / ukryty
    void setVisible(const NewChartSerie* serie, bool visible);

protected:
    //! prosta struktura zawieraj¹ca informacje o etykiecie
    struct LabelData
    {
        //! obiekt etykiety
        NewChartLabel* label;
        //! pierwsze po³¹cznie etykiety 
        NewChartDot* dot1;
        //! drugie po³¹czenie etykiety (mo¿e byæ puste)
        NewChartDot* dot2;
        //! seria zwiazana z etykieta
        const NewChartSerie* serie;
    };
    typedef core::shared_ptr<LabelData> LabelDataPtr;
    typedef core::shared_ptr<const LabelData> LabelDataConstPtr;

    typedef boost::tuple<const NewChartSerie*, QPointF, double> SeriePointDist;

protected:
    //! Realizuje przesuniecie pojedynczej etykiety
    //! \param pos nowa pozycja
    //! \param curve krzywa pod która jest podpiêta etykieta
    //! \param label przesuwana etykieta
    void move(const QPoint& pos, const QwtPlotCurve* curve, NewChartLabel* label);
    //! próbuje pobraæ dane etykiety
    //! \param pos pozycja na wykresie (wspó³rzêdne ekranowe)
    //! \return dane etykiety lub pusty wskaŸnik
    LabelDataConstPtr getLabel(const QPoint& pos);
    //! próbuje pobraæ dane etykiety
    //! \param pos pozycja na wykresie (wspó³rzêdne ekranowe)
    //! \param curve krzywa zwiazana z etykieta
    //! \return dane etykiety lub pusty wskaŸnik
    LabelDataConstPtr getLabel(const QPoint& pos, const QwtPlotCurve* curve);
    //! próbuje pobraæ dane etykiety
    //! \param label etykieta, dla której pobierane s¹ dane
    //! \return dane etykiety lub pusty wskaŸnik
    LabelDataConstPtr getLabel(const NewChartLabel* label) const;
    //! Pobiera najblizszy punkt od klikniecia
    //! \param pos pozycja klikniecia
    //! \return krotka <najblizsza seria, punkt na niej, odleg³oœæ od punktu klikniecia>
    SeriePointDist getClosestPoint(const QPoint& pos) const;

protected:
    //! wszystkie etykiety zwi¹zane ze stanem wizualizatora
    std::vector<LabelDataPtr> labels;

};
typedef core::shared_ptr<NewChartLabelState> NewChartLabelStatePtr;
typedef core::shared_ptr<const NewChartLabelState> NewChartLabelStateConstPtr;


#endif
