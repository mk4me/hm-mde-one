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
//! dziêki niej mo¿na zasilic wykres bez koniecznoœci kopiowania danych
//! Dodatkowo dodana zosta³a mo¿liwoœæ transformacji wykresu (offset + skala)
class NewChartSeriesData : public QwtSeriesData<QPointF>
{
public:
    //! Konstruktor, trzeba dostarczyæ dane do wykresu
    //! \param channel DataChannel, bêdzie przechowywany na w³asnoœæ
    //! \param offset pocz¹tkowe przesuniecie (domyslnie = (0,0))
    //! \param scaleX pocz¹tkowa skala w poziomie (domyslnie 1)
    //! \param scaleY pocz¹tkowa skala w pionie (domyslnie 1)
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
    //! 
    QPointF getOffset() const { return offset; }
    //! 
    //! \param val 
    void setOffset(QPointF val) { offset = val; }
    //! 
    double getScaleY() const { return scaleY; }
    //! 
    //! \param val 
    void setScaleY(double val) { scaleY = val; }
    //! 
    double getScaleX() const { return scaleX; }
    //! 
    //! \param val 
    void setScaleX(double val) { scaleX = val; }

private:
    //! DataChannel z danymi
    ScalarChannelReaderInterfaceConstPtr channel;
    //! ustawione przesuniecie
    QPointF offset;
    //! ustawiona skala pozioma
    double scaleX;
    //! ustawiona skala pionowa
    double scaleY;
};



#endif
