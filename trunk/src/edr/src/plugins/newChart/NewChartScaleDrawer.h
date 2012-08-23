/********************************************************************
	created:	2012/02/22
	created:	22:2:2012   10:22
	filename: 	NewChartScaleDrawer.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__NEWCHARTSCALEDRAWER_H__
#define HEADER_GUARD_NEW_CHART__NEWCHARTSCALEDRAWER_H__

//! Klasa umo¿liwia odrysowanie podzialki z procentami
//! nale¿y pamiêtaæ, ze w zaden sposób nie s¹ zmieniane wartoœci krzywych, podmieniana jest jedynie podzialka
class PercentScaleDraw : public QwtScaleDraw
{
public:
    //! Konstruktor wymaga podania przedzia³ów wykresu
    //! \param left minimalny argument aktualnie wyœwietlanego wykresu
    //! \param right maskymalny argument aktualnie wyœwietlanego wykresu 
    //! \param percentMode czy wyœwietlaæ podzialke z procentami czy podzialke zwykla
    PercentScaleDraw(double left, double right, bool percentMode = false);
    virtual ~PercentScaleDraw() {}

public:
    //! Usawia przedzia³y aktyalnie wyœwietlanego wykresu
    //! \param left minimalny argument aktualnie wyœwietlanego wykresu
    //! \param right maskymalny argument aktualnie wyœwietlanego wykresu 
    void setLeftRightValues(double left, double right);
    //! \return czy wyœwietlana podzialka z procentami czy podzialka zwykla
    bool isPercentMode() const { return percentMode; }
    //! Ustawienie podzialki
    //! \param val true - podzialka z procentami, false - zwykla podzialka
    void setPercentMode(bool val) { percentMode = val; }
    //! \return obiekt wykorzystywany przez Qwt do rysowania podzialki utworzony na podstawie stanu klasy
    QwtScaleDiv getScaleDiv();

protected:
    //! Rysuje etykiete dla konkretnego argumentu (np. 50 %)
    //! \param painter obiekt paintera
    //! \param value argument dla ktorego ma byæ narysowana etykieta
    void drawLabel( QPainter *painter, double value ) const;

private:
    //! minimalny argument aktualnie wyœwietlanego wykresu (przedzia³ lewy)
    double left;
    //! maksymalny argument aktualnie wyœwietlanego wykresu (przedzia³ prawy)
    double right;
    //! czy wyœwietlaæ podzialke z procentami czy podzialke zwykla
    bool percentMode;
};

#endif
