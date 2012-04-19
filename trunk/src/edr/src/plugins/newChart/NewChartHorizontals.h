/********************************************************************
	created:	2011/11/25
	created:	25:11:2011   9:00
	filename: 	NewChartHorizontals.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__NEWCHARTHORIZONTALS_H__
#define HEADER_GUARD_NEW_CHART__NEWCHARTHORIZONTALS_H__

#include "NewChartState.h"

class NewChartHorizontals : public NewChartState
{
public:
    NewChartHorizontals(NewChartVisualizer* visualizer) : NewChartState(visualizer) {}
	virtual ~NewChartHorizontals() {}

public:
    virtual bool stateEventFilter( QObject *, QEvent * );

    virtual void draw( QPainter * );
    virtual void stateBegin() {}
    virtual void stateEnd() {}

};
typedef core::shared_ptr<NewChartHorizontals> NewChartHorizontalsPtr;
typedef core::shared_ptr<const NewChartHorizontals> NewChartHorizontalsConstPtr;


#endif
