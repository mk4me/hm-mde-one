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

class NewChartSeriesData : public QwtSeriesData<QPointF>
{
public:
    NewChartSeriesData(ScalarChannelReaderInterfaceConstPtr channel, 
        QPointF offset = QPointF(), double scaleX = 1.0, double scaleY = 1.0);

public:
    virtual size_t size() const;
    virtual QPointF sample( size_t i ) const;
    virtual QRectF boundingRect() const;

public:
    QPointF getOffset() const { return offset; }
    void setOffset(QPointF val) { offset = val; }

    double getScaleY() const { return scaleY; }
    void setScaleY(double val) { scaleY = val; }
    double getScaleX() const { return scaleX; }
    void setScaleX(double val) { scaleX = val; }

private:
    ScalarChannelReaderInterfaceConstPtr channel;
    QPointF offset;
    double scaleX;
    double scaleY;
};

class NewChartEventStateData : public QwtSeriesData<QPointF>
{
public:
    NewChartEventStateData(ScalarChannelReaderInterfaceConstPtr channel, float startTime, float endTime);

public:
    virtual size_t size() const;
    virtual QPointF sample( size_t i ) const;
    virtual QRectF boundingRect() const;

private:
    ScalarChannelReaderInterfaceConstPtr channel;
    int startIndex;
    int endIndex;
};

#endif
