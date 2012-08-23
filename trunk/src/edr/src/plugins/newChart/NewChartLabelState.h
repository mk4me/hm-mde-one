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

//! Stan wykresu, kt�ry operuje w jakis spos�b na etykietach
class NewChartLabelState : public NewChartState
{
public:
    //! Konstruktor pobiera wizualizator, na kt�rym stan b�dzie operowa�
    NewChartLabelState(NewChartVisualizer* visualizer);
	virtual ~NewChartLabelState() {}

public:
    //! w tej klasie metoda jeszcze nie jest wykorzystywana
    virtual void draw(QPainter *);
    //! ustawienie stanu, zmiana kursora
    virtual void stateBegin();
    //! zako�czenie stanu, nieobs�ugiwane
    virtual void stateEnd();
    //! usuwa etykiety z wykresu zwi�zane z konkretna seria
    //! \param serie seria, dla kt�rej usuwamy
    void removeSerieLabels(const NewChartSerie* serie);
    //! zarz�dza widoczno�ci� etykiet z wykresu zwiazanych z konkretna seria
    //! \param serie seria, dla kt�rej zmieniamy widoczno�� element�w
    //! \param visible widoczny / ukryty
    void setVisible(const NewChartSerie* serie, bool visible);

protected:
    //! prosta struktura zawieraj�ca informacje o etykiecie
    struct LabelData
    {
        //! obiekt etykiety
        NewChartLabel* label;
        //! pierwsze po��cznie etykiety 
        NewChartDot* dot1;
        //! drugie po��czenie etykiety (mo�e by� puste)
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
    //! \param curve krzywa pod kt�ra jest podpi�ta etykieta
    //! \param label przesuwana etykieta
    void move(const QPoint& pos, const QwtPlotCurve* curve, NewChartLabel* label);
    //! pr�buje pobra� dane etykiety
    //! \param pos pozycja na wykresie (wsp�rz�dne ekranowe)
    //! \return dane etykiety lub pusty wska�nik
    LabelDataConstPtr getLabel(const QPoint& pos);
    //! pr�buje pobra� dane etykiety
    //! \param pos pozycja na wykresie (wsp�rz�dne ekranowe)
    //! \param curve krzywa zwiazana z etykieta
    //! \return dane etykiety lub pusty wska�nik
    LabelDataConstPtr getLabel(const QPoint& pos, const QwtPlotCurve* curve);
    //! pr�buje pobra� dane etykiety
    //! \param label etykieta, dla kt�rej pobierane s� dane
    //! \return dane etykiety lub pusty wska�nik
    LabelDataConstPtr getLabel(const NewChartLabel* label) const;
    //! Pobiera najblizszy punkt od klikniecia
    //! \param pos pozycja klikniecia
    //! \return krotka <najblizsza seria, punkt na niej, odleg�o�� od punktu klikniecia>
    SeriePointDist getClosestPoint(const QPoint& pos) const;

protected:
    //! wszystkie etykiety zwi�zane ze stanem wizualizatora
    std::vector<LabelDataPtr> labels;

};
typedef core::shared_ptr<NewChartLabelState> NewChartLabelStatePtr;
typedef core::shared_ptr<const NewChartLabelState> NewChartLabelStateConstPtr;


#endif
