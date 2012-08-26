/********************************************************************
    created:  2011/04/04
    created:  4:4:2011   17:25
    filename: ChartWidget.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CHART__CHARTVISUALIZERTITLEBAR_H__
#define HEADER_GUARD_CHART__CHARTVISUALIZERTITLEBAR_H__

#include <osgui/QOsgWidgets.h>
#include "Chart.h"
#include "ui_ChartVisualizerTitleBar.h"

class ChartWidget : public osgui::QOsgDefaultWidget, private Ui::ChartWidget
{
    Q_OBJECT;
private:
    //! Faktyczny wykres.
    osg::ref_ptr<Chart> chart;
    //! 
    int prevActiveSerie;
    //!
    float prevTime;

public:
    ChartWidget();
    virtual ~ChartWidget();

public:
    bool isShowingNormalized() const;

public slots:
    void setShowNormalized(bool normalized);
};

#endif  // HEADER_GUARD_CHART__CHARTVISUALIZERTITLEBAR_H__
