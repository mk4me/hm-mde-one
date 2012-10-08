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
#include <QtGui/QComboBox>
#include <osgViewer/Viewer>
#include <core/IVisualizer.h>
#include "Chart.h"
#include "LineChartSerie.h"
#include <osgui/QOsgWidgets.h>

//! Wizualizator wykresów.
class ChartVisualizer : public QObject, public core::IVisualizer
{
public:

    class ChartVisualizerSerie : public TimeSerieBase
    {
        friend class ChartVisualizer;

    private:
        ChartVisualizerSerie(const LineChartSeriePtr & serie, ChartVisualizer * visualizer)
            : serie(serie), visualizer(visualizer)
        {
       
        }

    public:

        const LineChartSeriePtr & getSerie() const
        {
            return serie;
        }

        //! \return Dlugosc kanalu w sekundach
        virtual float getLength() const
        {
            return (float)serie->getData()->getLength();
        }

        //! Czas zawiera się między 0 a getLength()
        //! \param time Aktualny, lokalny czas kanalu w sekundach
        virtual void setTime(float time)
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

        virtual void setName(const std::string & name)
        {
            //TODO
            //obecnie nazwy serii nie sa obslugiwane ale musimy pomyslec o tym i ewentualnie dodac!!
            this->name = name;
        }

        virtual const std::string & getName() const
        {
            return name;
        }

        virtual void setData(const core::ObjectWrapperConstPtr & data)
        {
            this->data = data;
            serie->setData(data->get());
        }

        virtual const core::ObjectWrapperConstPtr & getData() const
        {
            return data;
        }

        

    private:
        core::ObjectWrapperConstPtr data;
        LineChartSeriePtr serie;
        ChartVisualizer * visualizer;
        std::string name;
    };

    friend class ChartVisualizerSerie;

    Q_OBJECT;
    UNIQUE_ID("{68C4E6D6-5EC8-4641-8845-F3FF3766B709}", "Chart Visualizer");
private:

    //! Nazwa wizualizatora.
    std::string name;
    //! Viewer osg.
    osg::ref_ptr<osgui::QOsgDefaultWidget> viewer;
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

    QComboBox* activeSerieCombo;

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
    virtual ChartVisualizerSerie* createSerie(const core::ObjectWrapperConstPtr & data, const std::string & name = std::string());

    virtual ChartVisualizerSerie* createSerie(const SerieBase * serie);

    //! \param serie Seria danych do usunięcia, nie powinien usuwac tej serii! Zarzadzamy nia my!!
    virtual void removeSerie(core::IVisualizer::SerieBase* serie);

    virtual void reset();

    //! \return Czy wykres sam się odświeża przy zmianie?
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

    //! \return Czy pokazywać tytuł
    virtual bool isShowingTitle() const;
    //! \param, showTitle Czy pokazywać tytuł
    virtual void setShowTitle(bool showTitle);

    //! \return Tytuł wykresu
    virtual std::string getTitleText() const;
    //! \param titleText Tytuł wykresu
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

    virtual bool isShowingUnitsInCursor() const;
    virtual void setShowingUnitsInCursor(bool showUnits);

    virtual bool isShowingCursorBackground() const;
    virtual void setShowCursorBackground(bool showBackground);

    virtual osg::Vec4 getCursorBackgroundColor() const;
    virtual void setCursorBackgroundColor(osg::Vec4 color);

    virtual QPixmap print() const 
    {
        return QPixmap();
    }

    //! \return Prototyp labelek.
    virtual const osgText::Text* getLabelPrototype() const;
    //! \param prototype Prototyp labelek. Obiekt nie jest kopiowany. Gdy wprowadza się zmiany w labelce trzeba
    //! koniecznie wywołać jeszcze raz setLabelPrototype, bo inaczej nie będą odświeżone. Można również sklonować
    //! tekst w parametrze.
    virtual void setLabelPrototype(osgText::Text* prototype);
    
    virtual void setBackgroundColor(osg::Vec4 color);

    virtual osg::Vec4 setBackgroundColor() const;

    virtual bool isShowingTimeInCursor() const;
    virtual void setShowingTimeInCursor(bool showTime);

private slots:
    void setNormalized(bool normalized);
    void setActiveSerie(int idx);

signals:
    void activeSeriePicked(int idx);

};


#endif  // HEADER_GUARD_CHART__CHARTVISUALIZER_H__
