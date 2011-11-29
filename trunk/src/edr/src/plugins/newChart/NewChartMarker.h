/********************************************************************
	created:	2011/11/22
	created:	22:11:2011   12:30
	filename: 	NewChartMarker.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__NEWCHARTMARKER_H__
#define HEADER_GUARD_NEW_CHART__NEWCHARTMARKER_H__

#include <qwt/qwt_plot_marker.h>

class NewChartDot : public QwtPlotItem
{
public:
    NewChartDot(const QPointF& position, int size = 5);

public:
    virtual void draw( QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRectF &canvasRect ) const;

    const QPointF& getPosition() const { return position; }
    void setPosition(const QPointF& val) { position = val; }

    const QBrush& getBrush() const { return brush; }
    void setBrush(const QBrush& val) { brush = val; }

    const QPen& getPen() const { return pen; }
    void setPen(const QPen& val) { pen = val; }

    virtual int rtti() const { return QwtPlotItem::Rtti_PlotUserItem; }

private:
    int size;
    QPointF position;
    QPen pen;
    QBrush brush;
};
typedef core::shared_ptr<NewChartDot> NewChartDotPtr;
typedef core::shared_ptr<const NewChartDot> NewChartDotConstPtr;


class NewChartLabel : public QwtPlotItem
{
public:
    enum ConnectionStyle
    {
        Simple,
        Horizontal,
        Vertical
    };
public:
    NewChartLabel(const QString& text = QString(""), const QPoint& shift = QPoint(20, 20), const QPoint& size = QPoint(80, 35));

public:
    virtual void draw( QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRectF &canvasRect ) const;

    bool isInsideLabel(const QPoint& transformedPoint, const QwtPlotCurve* curve) const;
    
    const QString& getText() const { return text; }
    void setText(const QString& val) { text = val; }
    
    const QPoint& getShift() const { return shift; }
    void setShift(const QPoint& val) { shift = val; }
    

    NewChartDotConstPtr getPoint1() const { return point1; }
    QPoint getPoint1Transformed(const QwtPlotCurve* curve) const;
    void setPoint1(NewChartDotConstPtr val) { point1 = val; }
    NewChartDotConstPtr getPoint2() const { return point2; }
    QPoint getPoint2Transformed(const QwtPlotCurve* curve) const;
    void setPoint2(NewChartDotConstPtr val) { point2 = val; }
    
    const QBrush& getBrush() const { return brush; }
    void setBrush(const QBrush& val) { brush = val; }

    const QPen& getPen() const { return pen; }
    void setPen(const QPen& val) { pen = val; }

    virtual int rtti() const { return QwtPlotItem::Rtti_PlotUserItem; }

    void connectDot(NewChartDotConstPtr dot, ConnectionStyle style = Simple);
    void connectDots(NewChartDotConstPtr point1, NewChartDotConstPtr point2, ConnectionStyle style );

    void connectDots(const QPointF& point1, const QPointF& point2, ConnectionStyle style);

private:
    void drawConnection(QPainter* painter, const QPoint& transformedFrom, const QPoint& transformedTo, ConnectionStyle style) const;
    void drawArrow(QPainter* painter, const QPoint& transformedFrom, const QPoint& transformedTo) const;

private:
    QString text;
    QPoint size;
    QPoint shift;
    QPen pen;
    QBrush brush;
    NewChartDotConstPtr point1;
    NewChartDotConstPtr point2;
    ConnectionStyle connectionStyle;
};
typedef core::shared_ptr<NewChartLabel> NewChartLabelPtr;
typedef core::shared_ptr<const NewChartLabel> NewChartLabelConstPtr;

class NewChartMarker : public QwtPlotMarker
{
public:
    explicit NewChartMarker(ScalarChannelReaderInterfaceConstPtr reader);
    explicit NewChartMarker();
	virtual ~NewChartMarker() {}

protected:
    virtual void drawLines( QPainter *, const QRectF &, const QPointF & ) const;
    virtual void drawLabel( QPainter *, const QRectF &, const QPointF & ) const;

public:
    static void drawDot( QPainter * painter, const QPointF & point, int size = 5 );
    void resetMomentum() const { positionSet = false; }

private:
    ScalarChannelReaderInterfaceConstPtr reader;
    mutable QPointF position;
    mutable bool positionSet;
};
typedef core::shared_ptr<NewChartMarker> NewChartMarkerPtr;
typedef core::shared_ptr<const NewChartMarker> NewChartMarkerConstPtr;


#endif