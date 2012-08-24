/********************************************************************
	created:	2012/02/22
	created:	22:2:2012   10:22
	filename: 	NewChartScaleDrawer.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__NEWCHARTSCALEDRAWER_H__
#define HEADER_GUARD_NEW_CHART__NEWCHARTSCALEDRAWER_H__

//! Klasa umo�liwia odrysowanie podzia�ki z procentami
//! nale�y pami�ta�, ze w zaden spos�b nie s� zmieniane warto�ci krzywych, podmieniana jest jedynie podzia�ka
class PercentScaleDraw : public QwtScaleDraw
{
public:
    //! Konstruktor wymaga podania przedzia��w wykresu
    //! \param left minimalny argument aktualnie wy�wietlanego wykresu
    //! \param right maskymalny argument aktualnie wy�wietlanego wykresu 
    //! \param percentMode czy wy�wietla� podzia�k� z procentami czy podzia�k� zwyk��
    PercentScaleDraw(double left, double right, bool percentMode = false);
    virtual ~PercentScaleDraw() {}

public:
    //! Usawia przedzia�y aktyalnie wy�wietlanego wykresu
    //! \param left minimalny argument aktualnie wy�wietlanego wykresu
    //! \param right maskymalny argument aktualnie wy�wietlanego wykresu 
    void setLeftRightValues(double left, double right);
    //! \return czy wy�wietlana podzia�ka z procentami czy podzia�ka zwykla
    bool isPercentMode() const { return percentMode; }
    //! Ustawienie podzia�ki
    //! \param val true - podzia�ka z procentami, false - zwykla podzia�ka
    void setPercentMode(bool val) { percentMode = val; }
    //! \return obiekt wykorzystywany przez Qwt do rysowania podzia�ki utworzony na podstawie stanu klasy
    QwtScaleDiv getScaleDiv();

protected:
    //! Rysuje etykiet� dla konkretnego argumentu (np. 50 %)
    //! \param painter obiekt paintera
    //! \param value argument dla kt�rego ma by� narysowana etykieta
    void drawLabel( QPainter *painter, double value ) const;

private:
    //! minimalny argument aktualnie wy�wietlanego wykresu (przedzia� lewy)
    double left;
    //! maksymalny argument aktualnie wy�wietlanego wykresu (przedzia� prawy)
    double right;
    //! czy wy�wietla� podzia�k� z procentami czy podzia�k� zwyk��
    bool percentMode;
};

#endif
