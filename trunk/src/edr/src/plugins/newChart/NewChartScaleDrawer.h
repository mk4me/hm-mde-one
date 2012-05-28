/********************************************************************
	created:	2012/02/22
	created:	22:2:2012   10:22
	filename: 	NewChartScaleDrawer.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__NEWCHARTSCALEDRAWER_H__
#define HEADER_GUARD_NEW_CHART__NEWCHARTSCALEDRAWER_H__

//! Klasa umozliwia odrysowanie podzialki z procentami
//! Nalezy pamietac, ze w zaden sposob nie sa zmieniane wartosci krzywych, podmieniana jest jedynie podzialka
class PercentScaleDraw : public QwtScaleDraw
{
public:
    //! Konstruktor wymaga podania przedzialow wykresu
    //! \param left minimalny argument aktualnie wyswietlanego wykresu
    //! \param right maskymalny argument aktualnie wyswietlanego wykresu 
    //! \param percentMode czy wyswietlac podzialke z procentami czy podzialke zwykla
    PercentScaleDraw(double left, double right, bool percentMode = false);
    virtual ~PercentScaleDraw() {}

public:
    //! Usawia przedzialy aktyalnie wyswietlanego wykresu
    //! \param left minimalny argument aktualnie wyswietlanego wykresu
    //! \param right maskymalny argument aktualnie wyswietlanego wykresu 
    void setLeftRightValues(double left, double right);
    //! \return czy wyswietlana podzialka z procentami czy podzialka zwykla
    bool isPercentMode() const { return percentMode; }
    //! Ustawienie podzialki
    //! \param val true - podzialka z procentami, false - zwykla podzialka
    void setPercentMode(bool val) { percentMode = val; }
    //! \return obiekt wykorzystywany przez Qwt do rysowania podzialki utworzony na podstawie stanu klasy
    QwtScaleDiv getScaleDiv();

protected:
    //! Rysuje etykiete dla konkretnego argumentu (np. 50 %)
    //! \param painter obiekt paintera
    //! \param value argument dla ktorego ma byc narysowana etykieta
    void drawLabel( QPainter *painter, double value ) const;

private:
    //! minimalny argument aktualnie wyswietlanego wykresu (przedzial lewy)
    double left;
    //! maksymalny argument aktualnie wyswietlanego wykresu (przedzial prawy)
    double right;
    //! czy wyswietlac podzialke z procentami czy podzialke zwykla
    bool percentMode;
};

#endif
