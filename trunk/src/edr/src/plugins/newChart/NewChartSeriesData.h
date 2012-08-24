/********************************************************************
	created:	2012/02/22
	created:	22:2:2012   10:14
	filename: 	NewChartSeriesData.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__NEWCHARTSERIESDATA_H__
#define HEADER_GUARD_NEW_CHART__NEWCHARTSERIESDATA_H__

#include <plugins/c3d/C3DChannels.h>

//! Klasa rozszerza podstawowy kontener próbek z Qwt
//! dziêki niej mo¿na zasiliæ wykres bez koniecznoœci kopiowania danych
//! Dodatkowo dodana zosta³a mo¿liwoœæ transformacji wykresu (offset + skala)
class NewChartSeriesData : public QwtSeriesData<QPointF>
{
public:
    //! Konstruktor, trzeba dostarczyæ dane do wykresu
    //! \param channel DataChannel, bêdzie przechowywany na w³asnoœæ
    //! \param offset pocz¹tkowe przesuniêcie (domyœlnie = (0,0))
    //! \param scaleX pocz¹tkowa skala w poziomie (domyœlnie 1)
    //! \param scaleY pocz¹tkowa skala w pionie (domyœlnie 1)
    NewChartSeriesData(ScalarChannelReaderInterfaceConstPtr channel, 
        QPointF offset = QPointF(), double scaleX = 1.0, double scaleY = 1.0);

public:
    //! \return liczba próbek
    virtual size_t size() const;
    //! Konwertuje próbkê z DataChannel na format obs³ugiwany przez Qwt
    //! \param i numer próbki
    //! \return wartoœæ konkretnej próbki
    virtual QPointF sample( size_t i ) const;
    //! \return wielkoœæ obszaru zajmowanego przez krzywa
    virtual QRectF boundingRect() const;

public:
    //! \return przesuniêcie wykresu
    QPointF getOffset() const { return offset; }
    //! ustawia przesuniêcie wykresu
    //! \param val nowe przesuniêcie
    void setOffset(QPointF val) { offset = val; }
    //! \return skala dla wartoœci
    double getScaleY() const { return scaleY; }
    //! ustawia skalê dla wartoœci
    //! \param val nowa skala, nie mo¿e byæ równa 0
    void setScaleY(double val) { scaleY = val; }
    //! \return skala dla argumentów
    double getScaleX() const { return scaleX; }
    //! ustawia skalê dla argumentów
    //! \param val nowa skala, nie mo¿e byæ równa 0
    void setScaleX(double val) { scaleX = val; }

private:
    //! DataChannel z danymi
    ScalarChannelReaderInterfaceConstPtr channel;
    //! ustawione przesuniêcie
    QPointF offset;
    //! ustawiona skala pozioma
    double scaleX;
    //! ustawiona skala pionowa
    double scaleY;
};



#endif
