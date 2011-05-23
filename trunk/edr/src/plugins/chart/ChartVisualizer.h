/********************************************************************
    created:  2011/03/25
    created:  25:3:2011   14:47
    filename: ChartVisualizer.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CHART__CHARTVISUALIZER_H__
#define HEADER_GUARD_CHART__CHARTVISUALIZER_H__

#include <QtCore/QObject>
#include <osgViewer/Viewer>
#include <core/Chart.h>
#include <core/IVisualizer.h>
#include <core/LineChartSerie.h>

//! Wizualizator wykresów.
class ChartVisualizer : public QObject, public core::IVisualizer
{
    Q_OBJECT;
    UNIQUE_ID("{68C4E6D6-5EC8-4641-8845-F3FF3766B709}", "Chart Visualizer");
private:

    //! Nazwa wizualizatora.
    std::string name;
    //! Viewer osg.
    osg::ref_ptr<osgViewer::Viewer> viewer;
    //! Kolory dla serii danych
    std::vector<osg::Vec4> seriesColors;
    //! Faktyczny wykres.
    osg::ref_ptr<Chart> chart;
    //!
    int prevActiveSerie;
    //!
    float prevTime;

    //!
    QAction* actionNormalized;

    //! Serie danych przyporz¹dkowane indeksom wejœæ.
    std::vector<core::LineChartSeriePtr> series;

    volatile bool needsRefresh;

public:
    //!
    ChartVisualizer();
    //!
    virtual ~ChartVisualizer();

public:
    //! \see IVisualizer::getName
    virtual const std::string& getName() const;
    //! \see IVisualizer::create
    virtual core::IVisualizer* createClone() const;
    //! \see IVisualizer::getSlotInfo
    //virtual void getInputInfo(int inputNo, std::string& name, core::ObjectWrapper::Types& types);
    virtual void getInputInfo(int inputNo, core::IInputDescription::InputInfo& info);
    //! Nic nie robi.
    //! \see IVisualizer::update
    virtual void update(double deltaTime);
    //! \see IVisualizer::createWidget
    virtual QWidget* createWidget(std::vector<QObject*>& actions);
    //!
    virtual QIcon* createIcon();
    //! \see IVisualizer::setUp
    virtual void setUp(core::IObjectSource* source);

    virtual void process(core::IObjectSource* source);

private slots:
    void setNormalized(bool normalized);
    void setActiveSerie(int idx);

signals:
    void activeSeriePicked(int idx);

};


#endif  // HEADER_GUARD_CHART__CHARTVISUALIZER_H__