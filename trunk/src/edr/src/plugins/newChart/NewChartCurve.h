/********************************************************************
	created:	2012/02/22
	created:	22:2:2012   10:14
	filename: 	NewChartCurve.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__NEWCHARTCURVE_H__
#define HEADER_GUARD_NEW_CHART__NEWCHARTCURVE_H__

class NewChartSeriesData;

class NewChartCurve : public QwtPlotCurve
{
public:
    explicit NewChartCurve( const QString &title = QString::null );

public:
    double getXScale() const { return xScale; }
    void setXScale(double val);

    double getYScale() const { return yScale; }
    void setYScale(double val);

    double getXOffset() const { return xOffset; }
    void setXOffset(double val); 

    void setOffset(const QPointF& point);
    QPointF getOffset() const;

    double getYOffset() const { return yOffset; }
    void setYOffset(double val); 

    void setSamples(NewChartSeriesData* data);

protected:
    class OffsetTransformation : public QwtScaleTransformation
    {
    public:
        OffsetTransformation(double offset) : 
          QwtScaleTransformation( Linear),
              offset(offset)
          {}
          virtual double xForm( double s, double s1, double s2, double p1, double p2 ) const;
          virtual QwtScaleTransformation *copy() const;
          virtual double invXForm( double p, double p1, double p2, double s1, double s2 ) const;
          double getOffset() const { return offset; }
          void setOffset(double val) { offset = val; }
    private:
        double offset;

    };
    virtual void drawCurve( QPainter *p, int style,
        const QwtScaleMap &xMap, const QwtScaleMap &yMap,
        const QRectF &canvasRect, int from, int to ) const;

private:
    double xOffset;
    double yOffset;
    double xScale;
    double yScale;
};

#endif
