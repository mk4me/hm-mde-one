/********************************************************************
	created:	2012/02/22
	created:	22:2:2012   10:22
	filename: 	NewChartScaleDrawer.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__NEWCHARTSCALEDRAWER_H__
#define HEADER_GUARD_NEW_CHART__NEWCHARTSCALEDRAWER_H__

//! Klasa umo�liwia odrysowanie podzialki z procentami
//! nale�y pami�ta�, ze w zaden spos�b nie s� zmieniane warto�ci krzywych, podmieniana jest jedynie podzialka
class PercentScaleDraw : public QwtScaleDraw
{
public:
    //! Konstruktor wymaga podania przedzia��w wykresu
    //! \param left minimalny argument aktualnie wy�wietlanego wykresu
    //! \param right maskymalny argument aktualnie wy�wietlanego wykresu 
    //! \param percentMode czy wy�wietla� podzialke z procentami czy podzialke zwykla
    PercentScaleDraw(double left, double right, bool percentMode = false);
    virtual ~PercentScaleDraw() {}

public:
    //! Usawia przedzia�y aktyalnie wy�wietlanego wykresu
    //! \param left minimalny argument aktualnie wy�wietlanego wykresu
    //! \param right maskymalny argument aktualnie wy�wietlanego wykresu 
    void setLeftRightValues(double left, double right);
    //! \return czy wy�wietlana podzialka z procentami czy podzialka zwykla
    bool isPercentMode() const { return percentMode; }
    //! Ustawienie podzialki
    //! \param val true - podzialka z procentami, false - zwykla podzialka
    void setPercentMode(bool val) { percentMode = val; }
    //! \return obiekt wykorzystywany przez Qwt do rysowania podzialki utworzony na podstawie stanu klasy
    QwtScaleDiv getScaleDiv();

protected:
    //! Rysuje etykiete dla konkretnego argumentu (np. 50 %)
    //! \param painter obiekt paintera
    //! \param value argument dla ktorego ma by� narysowana etykieta
    void drawLabel( QPainter *painter, double value ) const;

private:
    //! minimalny argument aktualnie wy�wietlanego wykresu (przedzia� lewy)
    double left;
    //! maksymalny argument aktualnie wy�wietlanego wykresu (przedzia� prawy)
    double right;
    //! czy wy�wietla� podzialke z procentami czy podzialke zwykla
    bool percentMode;
};

#endif
