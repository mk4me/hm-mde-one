/********************************************************************
	created:	2012/05/28
	created:	28:5:2012   10:19
	filename: 	NewChartEventStateData.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__NEWCHARTEVENTSTATEDATA_H__
#define HEADER_GUARD_NEW_CHART__NEWCHARTEVENTSTATEDATA_H__

#include <plugins/c3d/C3DChannels.h>

//! Klasa dostarcza danych do wizualizacji kawalka krzywej zwiazanego z eventem
class NewChartEventStateData : public QwtSeriesData<QPointF>
{
public:
    //! Konstruktor wymaga zasilenia obiektu w dane
    //! \param channel DataChannel z danymi dla krzywej
    //! \param startTime poczatkowy czas dla eventu
    //! \param endTime koncowy czas dla eventu
    NewChartEventStateData(ScalarChannelReaderInterfaceConstPtr channel, float startTime, float endTime);

public:
    //! \return liczba probek przedzialu
    virtual size_t size() const;
    //! Zwraca probke z przedzialu eventa
    //! \param i indeks probki
    //! \return wartosc probki
    virtual QPointF sample( size_t i ) const;
    //! Obszar otaczajacy krzywa
    virtual QRectF boundingRect() const;

private:
    //! DataChannel z danymi dla krzywej
    ScalarChannelReaderInterfaceConstPtr channel;
    //! indeks poczatkowego czas dla eventu
    int startIndex;
    //! indeks koncowego czas dla eventu
    int endIndex;
};

#endif
