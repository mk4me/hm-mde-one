/********************************************************************
	created:	2012/02/22
	created:	22:2:2012   10:22
	filename: 	NewChartScaleDrawer.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__NEWCHARTSCALEDRAWER_H__
#define HEADER_GUARD_NEW_CHART__NEWCHARTSCALEDRAWER_H__

//! Klasa umo¿liwia odrysowanie podzia³ki z procentami
//! nale¿y pamiêtaæ, ze w zaden sposób nie s¹ zmieniane wartoœci krzywych, podmieniana jest jedynie podzia³ka
class PercentScaleDraw : public QwtScaleDraw
{
public:
    //! Konstruktor wymaga podania przedzia³ów wykresu
    //! \param left minimalny argument aktualnie wyœwietlanego wykresu
    //! \param right maskymalny argument aktualnie wyœwietlanego wykresu 
    //! \param percentMode czy wyœwietlaæ podzia³kê z procentami czy podzia³kê zwyk³¹
    PercentScaleDraw(double left, double right, bool percentMode = false);
    virtual ~PercentScaleDraw() {}

public:
    //! Usawia przedzia³y aktyalnie wyœwietlanego wykresu
    //! \param left minimalny argument aktualnie wyœwietlanego wykresu
    //! \param right maskymalny argument aktualnie wyœwietlanego wykresu 
    void setLeftRightValues(double left, double right);
    //! \return czy wyœwietlana podzia³ka z procentami czy podzia³ka zwykla
    bool isPercentMode() const { return percentMode; }
    //! Ustawienie podzia³ki
    //! \param val true - podzia³ka z procentami, false - zwykla podzia³ka
    void setPercentMode(bool val) { percentMode = val; }
    //! \return obiekt wykorzystywany przez Qwt do rysowania podzia³ki utworzony na podstawie stanu klasy
    QwtScaleDiv getScaleDiv();

protected:
    //! Rysuje etykietê dla konkretnego argumentu (np. 50 %)
    //! \param painter obiekt paintera
    //! \param value argument dla którego ma byæ narysowana etykieta
    void drawLabel( QPainter *painter, double value ) const;

private:
    //! minimalny argument aktualnie wyœwietlanego wykresu (przedzia³ lewy)
    double left;
    //! maksymalny argument aktualnie wyœwietlanego wykresu (przedzia³ prawy)
    double right;
    //! czy wyœwietlaæ podzia³kê z procentami czy podzia³kê zwyk³¹
    bool percentMode;
};

#endif
