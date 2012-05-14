/********************************************************************
	created:	2011/12/11
	created:	11:12:2011   19:39
	filename: 	EMGFilter.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMM__EMGFILTER_H__
#define HEADER_GUARD_HMM__EMGFILTER_H__

#include "TreeItemHelper.h"

//! Klasa tworzy wizualizator wykresow, i wypelnia go przefiltrowanymi danymi EMG
class EMGFilterHelper : public NewChartItemHelper
{
public:
    EMGFilterHelper(const core::ObjectWrapperConstPtr& wrapper) : NewChartItemHelper(wrapper) { }
    virtual ~EMGFilterHelper() {}

public:
    virtual VisualizerPtr createVisualizer();
    virtual void createSeries(const VisualizerPtr & visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series);
};
typedef core::shared_ptr<EMGFilterHelper> EMGFilterHelperPtr;
typedef core::shared_ptr<const EMGFilterHelper> EMGFilterHelperConstPtr;
#endif
