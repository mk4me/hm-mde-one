/********************************************************************
    created:  2011/03/25
    created:  25:3:2011   14:47
    filename: ChartVisualizer.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CHART__CHARTVISUALIZER_H__
#define HEADER_GUARD_CHART__CHARTVISUALIZER_H__

#include <core/IObjectSource.h>
#include <QtCore/QObject>
#include <osgViewer/Viewer>
#include <core/IVisualizer.h>
#include "Chart.h"
#include "LineChartSerie.h"

//! Wizualizator wykresów.
class ChartVisualizer : public QObject, public core::IVisualizer
{
private:

    class ChartVisualizerSerie : public SerieBase
    {
    public:
        ChartVisualizerSerie(const LineChartSeriePtr & serie)
            : serie(serie)
        {
       
        }

        const LineChartSeriePtr & getSerie() const
        {
            return serie;
        }

    protected:
        virtual void setSerieName(const std::string & name)
        {
            //TODO
            //obecnie nazwy serii nie sa obslugiwane ale musimy pomyslec o tym i ewentualnie dodac!!
        }

        virtual void setSerieData(const core::ObjectWrapperConstPtr & data)
        {
            serie->setData(data->get());
        }

    private:
        LineChartSeriePtr serie;
    };

    Q_OBJECT;
    UNIQUE_ID("{68C4E6D6-5EC8-4641-8845-F3FF3766B709}", "Chart Visualizer");
private:

    //! Nazwa wizualizatora.
    std::string name;
    //! Viewer osg.
    //osg::ref_ptr<osgViewer::Viewer> viewer;
    osg::ref_ptr<osgui::QOsgDefaultWidget> viewer;
    //! Kolory dla serii danych
    std::vector<osg::Vec4> seriesColors;
    //! Faktyczny wykres.
    osg::ref_ptr<Chart> chart;
    //!
    int prevActiveSerie;
    //!
    float prevTime;

    std::set<ChartVisualizerSerie*> series;

    //!
    QAction* actionNormalized;

    //! Serie danych przyporz¹dkowane indeksom wejœæ.
    //std::vector<LineChartSeriePtr> series;

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
    virtual void getInputInfo( std::vector<core::IInputDescription::InputInfo>& info);
    //! Nic nie robi.
    //! \see IVisualizer::update
    virtual void update(double deltaTime);
    //! \see IVisualizer::createWidget
    virtual QWidget* createWidget(std::vector<QObject*>& actions);
    //!
    virtual QIcon* createIcon();
    //! \see IVisualizer::setUp
    virtual void setUp(core::IObjectSource* source);

    //! \return Maksymalna ilosc serii danych jaka moze obsluzyc wizualizator, wartosc 0 i mniej oznacza maksumalny zakres dla INT, inna wartosc stanowi gorna granice
    virtual int getMaxDataSeries() const;

    //! \return Seria danych ktora mozna ustawiac - nazwa i dane, nie zarzadza ta seria danych - czasem jej zycia, my zwalniamy jej zasoby!!
    virtual core::IVisualizer::SerieBase* createSerie(const core::ObjectWrapperConstPtr & data, const std::string & name = std::string());

    //! \param serie Seria danych do usuniêcia, nie powinien usuwac tej serii! Zarzadzamy nia my!!
    virtual void removeSerie(core::IVisualizer::SerieBase* serie);

    virtual void reset();

private slots:
    void setNormalized(bool normalized);
    void setActiveSerie(int idx);

signals:
    void activeSeriePicked(int idx);

};


#endif  // HEADER_GUARD_CHART__CHARTVISUALIZER_H__