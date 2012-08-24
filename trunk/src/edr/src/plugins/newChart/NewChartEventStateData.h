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

//! Klasa dostarcza danych do wizualizacji kawa³ka krzywej zwi¹zanego z eventem
class NewChartEventStateData : public QwtSeriesData<QPointF>
{
public:
    //! Konstruktor wymaga zasilenia obiektu w dane
    //! \param channel DataChannel z danymi dla krzywej
    //! \param startTime pocz¹tkowy czas dla eventu
    //! \param endTime koñcowy czas dla eventu
    NewChartEventStateData(ScalarChannelReaderInterfaceConstPtr channel, float startTime, float endTime);

public:
    //! \return liczba próbek przedzia³u
    virtual size_t size() const;
    //! Zwraca próbkê z przedzia³u eventa
    //! \param i indeks próbki
    //! \return wartoœæ próbki
    virtual QPointF sample( size_t i ) const;
    //! Obszar otaczaj¹cy krzyw¹
    virtual QRectF boundingRect() const;

private:
    //! DataChannel z danymi dla krzywej
    ScalarChannelReaderInterfaceConstPtr channel;
    //! indeks pocz¹tkowego czas dla eventu
    int startIndex;
    //! indeks koñcowego czas dla eventu
    int endIndex;
};

#endif
