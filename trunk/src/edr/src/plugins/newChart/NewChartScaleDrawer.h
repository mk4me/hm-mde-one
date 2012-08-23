/********************************************************************
	created:	2012/02/22
	created:	22:2:2012   10:22
	filename: 	NewChartScaleDrawer.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__NEWCHARTSCALEDRAWER_H__
#define HEADER_GUARD_NEW_CHART__NEWCHARTSCALEDRAWER_H__

//! Klasa umożliwia odrysowanie podzialki z procentami
//! należy pamiętać, ze w zaden sposób nie są zmieniane wartości krzywych, podmieniana jest jedynie podzialka
class PercentScaleDraw : public QwtScaleDraw
{
public:
    //! Konstruktor wymaga podania przedziałów wykresu
    //! \param left minimalny argument aktualnie wyświetlanego wykresu
    //! \param right maskymalny argument aktualnie wyświetlanego wykresu 
    //! \param percentMode czy wyświetlać podzialke z procentami czy podzialke zwykla
    PercentScaleDraw(double left, double right, bool percentMode = false);
    virtual ~PercentScaleDraw() {}

public:
    //! Usawia przedziały aktyalnie wyświetlanego wykresu
    //! \param left minimalny argument aktualnie wyświetlanego wykresu
    //! \param right maskymalny argument aktualnie wyświetlanego wykresu 
    void setLeftRightValues(double left, double right);
    //! \return czy wyświetlana podzialka z procentami czy podzialka zwykla
    bool isPercentMode() const { return percentMode; }
    //! Ustawienie podzialki
    //! \param val true - podzialka z procentami, false - zwykla podzialka
    void setPercentMode(bool val) { percentMode = val; }
    //! \return obiekt wykorzystywany przez Qwt do rysowania podzialki utworzony na podstawie stanu klasy
    QwtScaleDiv getScaleDiv();

protected:
    //! Rysuje etykiete dla konkretnego argumentu (np. 50 %)
    //! \param painter obiekt paintera
    //! \param value argument dla ktorego ma być narysowana etykieta
    void drawLabel( QPainter *painter, double value ) const;

private:
    //! minimalny argument aktualnie wyświetlanego wykresu (przedział lewy)
    double left;
    //! maksymalny argument aktualnie wyświetlanego wykresu (przedział prawy)
    double right;
    //! czy wyświetlać podzialke z procentami czy podzialke zwykla
    bool percentMode;
};

#endif
