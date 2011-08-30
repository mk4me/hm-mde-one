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
#include <timelinelib/IChannel.h>
#include <osgui/QOsgWidgets.h>

//! Wizualizator wykresów.
class ChartVisualizer : public QObject, public core::IVisualizer
{
public:

    class ChartVisualizerSerie : public SerieBase, public timeline::IChannel
    {
    public:
        ChartVisualizerSerie(const LineChartSeriePtr & serie, ChartVisualizer * visualizer)
            : serie(serie), visualizer(visualizer)
        {
       
        }

        const LineChartSeriePtr & getSerie() const
        {
            return serie;
        }

        //! \return Sklonowane dane w kanale
        virtual timeline::IChannelPtr clone() const
        {
            //! NIE WSPIERAMY TUTAJ KLONOWANIA!!
            return timeline::IChannelPtr();
        }

        //! \return Dlugosc kanalu w sekundach
        virtual double getLength() const
        {
            return (double)serie->getData()->getLength();
        }

        //! Czas zawiera siê miêdzy 0 a getLength()
        //! \param time Aktualny, lokalny czas kanalu w sekundach
        virtual void setTime(double time)
        {
            serie->getTimer()->setTime(time);
        }

        virtual void setColor(osg::Vec4 color)
        {
            serie->setColor(color);
        }

        virtual osg::Vec4 getColor() const
        {
            return serie->getColor();
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
        ChartVisualizer * visualizer;
    };

    friend class ChartVisualizerSerie;

    Q_OBJECT;
    UNIQUE_ID("{68C4E6D6-5EC8-4641-8845-F3FF3766B709}", "Chart Visualizer");
private:

    //! Nazwa wizualizatora.
    std::string name;
    //! Viewer osg.
    osg::ref_ptr<osgui::QOsgDefaultWidget> viewer;
    //! Kolory dla serii danych
    std::vector<osg::Vec4> seriesColors;
    //! Faktyczny wykres.
    osg::ref_ptr<Chart> chart;
    //!
    int prevActiveSerie;
    //!
    float prevTime;
    //! Seriwe danych
    std::vector<ChartVisualizerSerie*> series;

    //!
    QAction* actionNormalized;

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

    //! \return Czy wykres sam siê odœwie¿a przy zmianie?
    virtual bool getAutoRefresh() const;
    //! \param autoRefresh
    virtual void setAutoRefresh(bool autoRefresh);

    //! zwraca odstep ramki od wykresu
    virtual float getMargin() const;
    //! ustawia odstep ramki od wykres
    virtual void setMargin(float margin);

    //! \return
    virtual bool isShowingFrame() const;
    //! \param showFrame
    virtual void setShowFrame(bool showFrame);
    //! \return
    virtual bool isShowingGridX() const;
    //! \param showGridX
    virtual void setShowGridX(bool showGridX);
    //! \return
    virtual bool isShowingGridY() const;
    //! \param showGridY
    virtual void setShowGridY(bool showGridY);
    //! pobiera gestosc siatki. Ile pixeli szerokosci ma jedna kratka
    virtual int getGridDensity() const;
    //! ustawia gestosc siatki. Ile pixeli szerokosci ma jedna kratka
    virtual void setGridDensity(int gridDensity);
    //! \return
    virtual osg::Vec4 getFrameColor() const;
    //! \param frameColor
    virtual void setFrameColor(osg::Vec4 frameColor);
    //! pobiera kolor siatki w rgba
    virtual osg::Vec4 getGridColor() const;
    //! ustawia kolor siatki w rgba
    virtual void setGridColor(osg::Vec4 gridColor);
    //! \return
    virtual float getFrameWidth() const;
    //! \param frameWidth
    virtual void setFrameWidth(float frameWidth);
    //! \return
    virtual float getGridDashWidth() const;
    //! \param dashWidth
    virtual void setGridDashWidth(float dashWidth);

    //! \return
    virtual bool isShowingAxisX() const;
    //! \param showAxisX
    virtual void setShowAxisX(bool showAxisX);
    //! \return
    virtual bool isAxisXInside() const;
    //! \param axisXInside
    virtual void setAxisXInside(bool axisXInside);
    //! \return
    virtual bool isShowingAxisY() const;
    //! \param showAxisY
    virtual void setShowAxisY(bool showAxisY);
    //! \return
    virtual bool isAxisYInside() const;
    //! \param axisXInside
    virtual void setAxisYInside(bool axisXInside);

    //! Pobiera kolor wykresu w rgba
    virtual osg::Vec4 getAxisesColor() const;
    //! Ustawia kolor wykresu w rgba
    virtual void setAxisesColor(osg::Vec4 color);

    //! Pobiera kolor kursora w rgba
    virtual osg::Vec4 getCursorColor() const;
    //! Ustawia kolor kursora w rgba
    virtual void setCursorColor(osg::Vec4 color);

    //! \return Czy pokazywaæ tytu³
    virtual bool isShowingTitle() const;
    //! \param, showTitle Czy pokazywaæ tytu³
    virtual void setShowTitle(bool showTitle);

    //! \return Tytu³ wykresu
    virtual std::string getTitleText() const;
    //! \param titleText Tytu³ wykresu
    virtual void setTitleText(const std::string & titleText);

    virtual int getTitleTextSize() const;
    virtual void setTitleTextSize(int titleTextSize);

    virtual Chart::TitlePosition getTitlePosition() const;
    virtual void setTitlePosition(Chart::TitlePosition position);

    virtual bool isShowingXUnitsSeparately() const;
    virtual void setShowingXUnitsSeparately(bool unitsSeparate);

    virtual bool isShowingYUnitsSeparately() const;
    virtual void setShowingYUnitsSeparately(bool unitsSeparate);

    virtual bool isShowingXUnits() const;
    virtual void setShowingXUnits(bool showUnits);

    virtual bool isShowingYUnits() const;
    virtual void setShowingYUnits(bool showUnits);

    //! \return Prototyp labelek.
    virtual const osgText::Text* getLabelPrototype() const;
    //! \param prototype Prototyp labelek. Obiekt nie jest kopiowany. Gdy wprowadza siê zmiany w labelce trzeba
    //! koniecznie wywo³aæ jeszcze raz setLabelPrototype, bo inaczej nie bêd¹ odœwie¿one. Mo¿na równie¿ sklonowaæ
    //! tekst w parametrze.
    virtual void setLabelPrototype(osgText::Text* prototype);
    
    virtual void setBackgroundColor(osg::Vec4 color);

    virtual osg::Vec4 setBackgroundColor() const;

private slots:
    void setNormalized(bool normalized);
    void setActiveSerie(int idx);

signals:
    void activeSeriePicked(int idx);

};


#endif  // HEADER_GUARD_CHART__CHARTVISUALIZER_H__