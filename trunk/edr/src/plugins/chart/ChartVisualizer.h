/********************************************************************
    created:  2011/03/25
    created:  25:3:2011   14:47
    filename: ChartVisualizer.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CHART__CHARTVISUALIZER_H__
#define HEADER_GUARD_CHART__CHARTVISUALIZER_H__

#include <osgViewer/Viewer>
#include <core/Chart.h>
#include <core/IVisualizer.h>

//! Wizualizator wykresów.
class ChartVisualizer : public core::IVisualizer
{
    UNIQUE_ID('CHRT','VISU')
private:
    //! Nazwa wizualizatora.
    std::string name;
    //! Viewer osg.
    osg::ref_ptr<osgViewer::Viewer> viewer;
    //! Kolory dla serii danych
    std::vector<osg::Vec4> seriesColors;
    //! Faktyczny wykres.
    osg::ref_ptr<Chart> chart;

public:
    //!
    ChartVisualizer();
    //!
    virtual ~ChartVisualizer();

public:
    //! \see IVisualizer::getName
    virtual const std::string& getName() const;
    //! \see IVisualizer::create
    virtual core::IVisualizer* create() const;
    //! \see IVisualizer::getSlotInfo
    virtual void getSlotInfo(int source, std::string& name, core::ObjectWrapper::Types& types);
    //! Nic nie robi.
    //! \see IVisualizer::update
    virtual void update(double deltaTime);
    //! \see IVisualizer::createWidget
    virtual QWidget* createWidget();
    //!
    virtual QIcon* createIcon();
    //! \see IVisualizer::setUp
    virtual void setUp(core::IObjectSource* source);
};


#endif  // HEADER_GUARD_CHART__CHARTVISUALIZER_H__