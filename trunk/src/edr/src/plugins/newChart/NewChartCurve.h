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

//! Klasa rozszerza podstawowa funkcjonalnoœæ Qwt przede wszystkim o mo¿liwoœæ transformacji (przesuniecie + skala)
//! pamiêtaæ nale¿y, ze nie jest to QObject, dlatego trzeba uwazac przy zarz¹dzaniu pamiecia
class NewChartCurve : public QwtPlotCurve
{
public:
    //! Konstruktor ustawia domyslne wartoœci (offset = 0, skala = 1)
    //! \param title tytul wykresu
    explicit NewChartCurve( const QString &title = QString::null );

public:
    //!
    double getXScale() const { return xScale; }
    //!
    //! \param val
    void setXScale(double val);
    //!
    double getYScale() const { return yScale; }
    //!
    //! \param val
    void setYScale(double val);
    //!
    double getXOffset() const { return xOffset; }
    //!
    //! \param val
    void setXOffset(double val);
    //!
    //! \param point
    void setOffset(const QPointF& point);
    //!
    QPointF getOffset() const;
    //!
    double getYOffset() const { return yOffset; }
    //!
    //! \param val
    void setYOffset(double val);
    //! Zasilenie krzywej w dane
    //! \param data Obiekt z danymi, stworzony na porzeby krzywej, przykrywa DataChannel
    void setSamples(NewChartSeriesData* data);

protected:
    //! Metoda jawnie wskazuje, ktorej metody u¿yæ (usuniecie warn. 'via dominance')
    virtual void dataChanged() { QwtPlotSeriesItem::dataChanged(); }
    //! Metoda jawnie wskazuje, ktorej metody u¿yæ (usuniecie warn. 'via dominance')
    virtual void setRectOfInterest( const QRectF& r ) { QwtSeriesStore<QPointF>::setRectOfInterest(r); }
    //! Metoda jawnie wskazuje, ktorej metody u¿yæ (usuniecie warn. 'via dominance')
    virtual size_t dataSize() const { return QwtSeriesStore<QPointF>::dataSize(); }
    //! Metoda jawnie wskazuje, ktorej metody u¿yæ (usuniecie warn. 'via dominance')
    virtual QRectF dataRect() const { return QwtSeriesStore<QPointF>::dataRect(); }

private:
    //! przesuniecie w poziomie
    double xOffset;
    //! przesuniecie w pionie
    double yOffset;
    //! skala w poziomie
    double xScale;
    //! skala w pionie
    double yScale;
};

#endif
