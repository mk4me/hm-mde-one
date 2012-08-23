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

//! Klasa rozszerza podstawowy kontener pr�bek z Qwt
//! dzi�ki niej mo�na zasilic wykres bez konieczno�ci kopiowania danych
//! Dodatkowo dodana zosta�a mo�liwo�� transformacji wykresu (offset + skala)
class NewChartSeriesData : public QwtSeriesData<QPointF>
{
public:
    //! Konstruktor, trzeba dostarczy� dane do wykresu
    //! \param channel DataChannel, b�dzie przechowywany na w�asno��
    //! \param offset pocz�tkowe przesuniecie (domyslnie = (0,0))
    //! \param scaleX pocz�tkowa skala w poziomie (domyslnie 1)
    //! \param scaleY pocz�tkowa skala w pionie (domyslnie 1)
    NewChartSeriesData(ScalarChannelReaderInterfaceConstPtr channel, 
        QPointF offset = QPointF(), double scaleX = 1.0, double scaleY = 1.0);

public:
    //! \return liczba pr�bek
    virtual size_t size() const;
    //! Konwertuje pr�bk� z DataChannel na format obs�ugiwany przez Qwt
    //! \param i numer pr�bki
    //! \return warto�� konkretnej pr�bki
    virtual QPointF sample( size_t i ) const;
    //! \return wielko�� obszaru zajmowanego przez krzywa
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
