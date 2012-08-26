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

//! Klasa tworzy wizualizator wykresów, i wypełnią go przefiltrowanymi danymi EMG
class EMGFilterHelper : public NewChartItemHelper
{
public:
    //! Konstruktor
    //! \param wrapper 
    EMGFilterHelper(const core::ObjectWrapperConstPtr& wrapper);
    virtual ~EMGFilterHelper() {}

public:
    //! \return wizualizator stworzony dla dostarczonego wrappera
    virtual VisualizerPtr createVisualizer();
    //! tworzy serie ze zmodyfikowanym EMG na podstawiawie dostarczonego wrappera
    //! \param visualizer wizualizator, który będzie tworzył serie
    //! \param path ścieżka dla timeline'a
    //! \param series zwracana seria 
    virtual void createSeries(const VisualizerPtr & visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series);
};
typedef core::shared_ptr<EMGFilterHelper> EMGFilterHelperPtr;
typedef core::shared_ptr<const EMGFilterHelper> EMGFilterHelperConstPtr;
#endif
