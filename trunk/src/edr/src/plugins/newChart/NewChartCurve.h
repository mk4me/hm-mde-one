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

//! Klasa rozszerza podstawowa funkcjonalnoœæ Qwt przede wszystkim o mo¿liwoœæ transformacji (przesuniêcie + skala)
//! pamiêtaæ nale¿y, ze nie jest to QObject, dlatego trzeba uwazac przy zarz¹dzaniu pamiecia
class NewChartCurve : public QwtPlotCurve
{
public:
    //! Konstruktor ustawia domyœlne wartoœci (offset = 0, skala = 1)
    //! \param title tytu³ wykresu
    explicit NewChartCurve( const QString &title = QString::null );

public:
    //! \return skala krzywej dla argumentów
    double getXScale() const { return xScale; }
    //! ustawia skalê krzywej dla argumentów, krzywa musi istnieæ
    //! \param val nowa skala
    void setXScale(double val);
    //! \return skala krzywej dla wartoœci
    double getYScale() const { return yScale; }
    //! ustawia skalê krzywej dla wartoœci, krzywa musi istnieæ
    //! \param val nowa skala
    void setYScale(double val);
    //! \return przesuniêcie krzywej dla argumentów
    double getXOffset() const { return xOffset; }
    //! ustawia przesuniêcie, krzywa musi istnieæ
    //! \param val przesuniêcie krzywej dla argumentów
    void setXOffset(double val);
    //! ustawia przesuniêcie, krzywa musi istnieæ
    //! \param point nowe przesuniêcie krzywej 
    void setOffset(const QPointF& point);
    //! \return pobiera przesuniêcie krzywej, krzywa musi istnieæ
    QPointF getOffset() const;
    //! \return przesuniêcie krzywej dla wartoœci
    double getYOffset() const { return yOffset; }
    //! ustawia przesuniêcie, krzywa musi istnieæ
    //! \param val przesuniêcie krzywej dla wartoœci
    void setYOffset(double val);
    //! Zasilenie krzywej w dane
    //! \param data Obiekt z danymi, stworzony na porzeby krzywej, przykrywa DataChannel
    void setSamples(NewChartSeriesData* data);

protected:
    //! jawne wskazanie, której metody u¿yæ (usuniêcie warn. 'via dominance')
    virtual void dataChanged() { QwtPlotSeriesItem::dataChanged(); }
    //! jawne wskazanie, której metody u¿yæ (usuniêcie warn. 'via dominance')
    virtual void setRectOfInterest( const QRectF& r ) { QwtSeriesStore<QPointF>::setRectOfInterest(r); }
    //! jawne wskazanie, której metody u¿yæ (usuniêcie warn. 'via dominance')
    virtual size_t dataSize() const { return QwtSeriesStore<QPointF>::dataSize(); }
    //! jawne wskazanie, której metody u¿yæ (usuniêcie warn. 'via dominance')
    virtual QRectF dataRect() const { return QwtSeriesStore<QPointF>::dataRect(); }

private:
    //! przesuniêcie w poziomie
    double xOffset;
    //! przesuniêcie w pionie
    double yOffset;
    //! skala w poziomie
    double xScale;
    //! skala w pionie
    double yScale;
};

#endif
