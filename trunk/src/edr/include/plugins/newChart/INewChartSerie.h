/********************************************************************
	created:	2012/04/30
	created:	30:4:2012   13:19
	filename: 	INewChartSerie.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__INEWCHARTSERIE_H__
#define HEADER_GUARD_NEW_CHART__INEWCHARTSERIE_H__

#include <plugins/c3d/EventSerieBase.h>

class INewChartSerie : public EventSerieBase
{
public:
	virtual ~INewChartSerie() {}

public:
    virtual QColor getColor() const = 0;
    virtual void setColor(const QColor& color) = 0;
};
typedef core::shared_ptr<INewChartSerie> INewChartSeriePtr;
typedef core::shared_ptr<const INewChartSerie> INewChartSerieConstPtr;


#endif
