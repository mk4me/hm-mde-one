/********************************************************************
	created:	2011/11/22
	created:	22:11:2011   12:30
	filename: 	NewChartMarker.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__NEWCHARTMARKER_H__
#define HEADER_GUARD_NEW_CHART__NEWCHARTMARKER_H__

#include <plugins/c3d/C3DChannels.h>

//! Marker pokazuj�cy aktualny czas i warto�� krzywej
//! Wykorzystuje ju� istniej�cy obiekt z Qwt
//! Dodatkowo wprowadza op�nienie przesuwania
class NewChartMarker : public QwtPlotMarker
{
public:
    //! konstruktor z podpi�tymi danymi
    explicit NewChartMarker(ScalarChannelReaderInterfaceConstPtr reader);
    explicit NewChartMarker();
	virtual ~NewChartMarker() {}

protected:
    //! dziedziczona z QwtPlotMarker. Odrysowanie liniii wskazuj�cych punkt
    virtual void drawLines( QPainter *, const QRectF &, const QPointF & ) const;
    //! dziedziczona z QwtPlotMarker. Odrysowanie etykiety
    virtual void drawLabel( QPainter *, const QRectF &, const QPointF & ) const;

public:
    //! rysuje punkt o okre�lonym rozmiarze
    static void drawDot( QPainter * painter, const QPointF & point, int size = 5 );
    //! resetuje 'ped'. Pozycja etykiety wynika z punktu i przesuni�cia
    void resetMomentum() const { positionSet = false; }
    //! ustawia wspoczynniki op�nienia
    //! \param x wsp�czynnik poziomy
    //! \param y wsp�czynnik pionowy
    void setLerpRatios(float x, float y) { lerpX = x; lerpY = y; }
    //! ustawia jednolity wsp�czynnik op�nienia
    //! \param lerp wsp�czynnik dla pionu i poziomu
    void setLerpRatios(float lerp) { lerpX = lerp; lerpY = lerp; }

private:
    //! wskazywane dane
    ScalarChannelReaderInterfaceConstPtr reader;
    //! aktualna pozycja etykiety
    mutable QPointF position;
    //! umo�liwia opoznione ustawienie pozycji
    mutable bool positionSet;
    //! poziomy wspoczynniki op�nienia
    float lerpX;
    //! pionowy wspoczynniki op�nienia
    float lerpY;
};
typedef core::shared_ptr<NewChartMarker> NewChartMarkerPtr;
typedef core::shared_ptr<const NewChartMarker> NewChartMarkerConstPtr;


#endif
