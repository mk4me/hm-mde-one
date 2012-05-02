/********************************************************************
	created:	2012/04/30
	created:	30:4:2012   13:19
	filename: 	INewChartVisualizer.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__INEWCHARTVISUALIZER_H__
#define HEADER_GUARD_NEW_CHART__INEWCHARTVISUALIZER_H__

class INewChartVisualizer : public core::IVisualizer
{
public:
	virtual ~INewChartVisualizer() {}

public:
    virtual void setTitle( const QString& title ) = 0;
    virtual QString getTitle() const = 0;
};
typedef core::shared_ptr<INewChartVisualizer> INewChartVisualizerPtr;
typedef core::shared_ptr<const INewChartVisualizer> INewChartVisualizerConstPtr;


#endif
