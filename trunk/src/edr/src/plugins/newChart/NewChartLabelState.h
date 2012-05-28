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

//! Stan wykresu, ktory operuje w jakis sposob na etykietach
class NewChartLabelState : public NewChartState
{
public:
    //! Konstruktor pobiera wizualizator, na ktorym stan bedzie operowal
    NewChartLabelState(NewChartVisualizer* visualizer);
	virtual ~NewChartLabelState() {}

public:
    //! w tej klasie metoda jeszcze nie jest wykorzystywana
    virtual void draw(QPainter *);
    //! ustawienie stanu, zmiana kursora
    virtual void stateBegin();
    //! zakonczenie stanu, nieobslugiwane
    virtual void stateEnd();
    //! usuwa etykiety z wykresu zwiazane z konkretna seria
    //! \param serie seria, dla ktorej usuwamy
    void removeSerieLabels(const NewChartSerie* serie);
    //! zarzadza widocznoscia etykiet z wykresu zwiazanych z konkretna seria
    //! \param serie seria, dla ktorej zmieniamy widocznosc elementow
    //! \param visible widoczny / ukryty
    void setVisible(const NewChartSerie* serie, bool visible);

protected:
    //! prosta struktura zawierajaca informacje o etykiecie
    struct LabelData
    {
        //! obiekt etykiety
        NewChartLabel* label;
        //! pierwsze polacznie etykiety 
        NewChartDot* dot1;
        //! drugie polaczenie etykiety (moze byc puste)
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
    //! \param curve krzywa pod ktora jest podpieta etykieta
    //! \param label przesuwana etykieta
    void move(const QPoint& pos, const QwtPlotCurve* curve, NewChartLabel* label);
    //! Probuje pobrac dane etykiety
    //! \param pos pozycja na wykresie (wspolrzedne ekranowe)
    //! \return dane etykiety lub pusty wskaznik
    LabelDataConstPtr getLabel(const QPoint& pos);
    //! Probuje pobrac dane etykiety
    //! \param pos pozycja na wykresie (wspolrzedne ekranowe)
    //! \param curve krzywa zwiazana z etykieta
    //! \return dane etykiety lub pusty wskaznik
    LabelDataConstPtr getLabel(const QPoint& pos, const QwtPlotCurve* curve);
    //! Probuje pobrac dane etykiety
    //! \param label etykieta, dla ktorej pobierane sa dane
    //! \return dane etykiety lub pusty wskaznik
    LabelDataConstPtr getLabel(const NewChartLabel* label) const;
    //! Pobiera najblizszy punkt od klikniecia
    //! \param pos pozycja klikniecia
    //! \return krotka <najblizsza seria, punkt na niej, odleglosc od punktu klikniecia>
    SeriePointDist getClosestPoint(const QPoint& pos) const;

protected:
    //! wszystkie etykiety zwiazane ze stanem wizualizatora
    std::vector<LabelDataPtr> labels;

};
typedef core::shared_ptr<NewChartLabelState> NewChartLabelStatePtr;
typedef core::shared_ptr<const NewChartLabelState> NewChartLabelStateConstPtr;


#endif
