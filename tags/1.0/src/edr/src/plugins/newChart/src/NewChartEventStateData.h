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

//! Klasa dostarcza danych do wizualizacji kawałka krzywej związanego z eventem
class NewChartEventStateData : public QwtSeriesData<QPointF>
{
public:
    //! Konstruktor wymaga zasilenia obiektu w dane
    //! \param channel DataChannel z danymi dla krzywej
    //! \param startTime początkowy czas dla eventu
    //! \param endTime końcowy czas dla eventu
    NewChartEventStateData(ScalarChannelReaderInterfaceConstPtr channel, float startTime, float endTime);

public:
    //! \return liczba próbek przedziału
    virtual size_t size() const;
    //! Zwraca próbkę z przedziału eventa
    //! \param i indeks próbki
    //! \return wartość próbki
    virtual QPointF sample( size_t i ) const;
    //! Obszar otaczający krzywą
    virtual QRectF boundingRect() const;

private:
    //! DataChannel z danymi dla krzywej
    ScalarChannelReaderInterfaceConstPtr channel;
    //! indeks początkowego czas dla eventu
    int startIndex;
    //! indeks końcowego czas dla eventu
    int endIndex;
};

#endif
