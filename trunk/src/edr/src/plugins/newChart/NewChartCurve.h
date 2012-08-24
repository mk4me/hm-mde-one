/********************************************************************
	created:	2012/02/22
	created:	22:2:2012   10:14
	filename: 	NewChartCurve.h
	author:		Wojciech Kniec

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__NEWCHARTCURVE_H__
#define HEADER_GUARD_NEW_CHART__NEWCHARTCURVE_H__

#include <qwt_plot_curve.h>

class NewChartSeriesData;

//! Klasa rozszerza podstawowa funkcjonalno�� Qwt przede wszystkim o mo�liwo�� transformacji (przesuni�cie + skala)
//! pami�ta� nale�y, ze nie jest to QObject, dlatego trzeba uwazac przy zarz�dzaniu pamiecia
class NewChartCurve : public QwtPlotCurve
{
public:
    //! Konstruktor ustawia domy�lne warto�ci (offset = 0, skala = 1)
    //! \param title tytu� wykresu
    explicit NewChartCurve( const QString &title = QString::null );

public:
    //! \return skala krzywej dla argument�w
    double getXScale() const { return xScale; }
    //! ustawia skal� krzywej dla argument�w, krzywa musi istnie�
    //! \param val nowa skala
    void setXScale(double val);
    //! \return skala krzywej dla warto�ci
    double getYScale() const { return yScale; }
    //! ustawia skal� krzywej dla warto�ci, krzywa musi istnie�
    //! \param val nowa skala
    void setYScale(double val);
    //! \return przesuni�cie krzywej dla argument�w
    double getXOffset() const { return xOffset; }
    //! ustawia przesuni�cie, krzywa musi istnie�
    //! \param val przesuni�cie krzywej dla argument�w
    void setXOffset(double val);
    //! ustawia przesuni�cie, krzywa musi istnie�
    //! \param point nowe przesuni�cie krzywej 
    void setOffset(const QPointF& point);
    //! \return pobiera przesuni�cie krzywej, krzywa musi istnie�
    QPointF getOffset() const;
    //! \return przesuni�cie krzywej dla warto�ci
    double getYOffset() const { return yOffset; }
    //! ustawia przesuni�cie, krzywa musi istnie�
    //! \param val przesuni�cie krzywej dla warto�ci
    void setYOffset(double val);
    //! Zasilenie krzywej w dane
    //! \param data Obiekt z danymi, stworzony na porzeby krzywej, przykrywa DataChannel
    void setSamples(NewChartSeriesData* data);

protected:
    //! jawne wskazanie, kt�rej metody u�y� (usuni�cie warn. 'via dominance')
    virtual void dataChanged() { QwtPlotSeriesItem::dataChanged(); }
    //! jawne wskazanie, kt�rej metody u�y� (usuni�cie warn. 'via dominance')
    virtual void setRectOfInterest( const QRectF& r ) { QwtSeriesStore<QPointF>::setRectOfInterest(r); }
    //! jawne wskazanie, kt�rej metody u�y� (usuni�cie warn. 'via dominance')
    virtual size_t dataSize() const { return QwtSeriesStore<QPointF>::dataSize(); }
    //! jawne wskazanie, kt�rej metody u�y� (usuni�cie warn. 'via dominance')
    virtual QRectF dataRect() const { return QwtSeriesStore<QPointF>::dataRect(); }

private:
    //! przesuni�cie w poziomie
    double xOffset;
    //! przesuni�cie w pionie
    double yOffset;
    //! skala w poziomie
    double xScale;
    //! skala w pionie
    double yScale;
};

#endif
