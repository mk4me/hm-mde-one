/********************************************************************
	created:  2011/03/10
	created:  10:3:2011   17:15
	filename: VisualizerWidget.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__VISUALIZERWIDGET_H__
#define HEADER_GUARD_CORE__VISUALIZERWIDGET_H__

#include <map>
#include <QtGui/QDockWidget>
#include "Visualizer.h"
#include "VisualizerManager.h"
#include <core/PluginCommon.h>
#include "EDRDockWidget.h"
#include <timelinelib/IChannel.h>

class IVisualizerChannel;

typedef boost::shared_ptr<IVisualizerChannel> IVisualizerChannelPtr;
typedef boost::shared_ptr<const IVisualizerChannel> IVisualizerChannelConstPtr;

class VisualizerChannel;

typedef boost::shared_ptr<VisualizerChannel> VisualizerChannelPtr;
typedef boost::shared_ptr<const VisualizerChannel> VisualizerChannelConstPtr;

//! Widget wizualizacyjny.
class VisualizerWidget : public EDRDockWidget
{
    Q_OBJECT

private:

    typedef std::map<core::VisualizerTimeSeriePtr, VisualizerChannelPtr> TimelineChannels;

private:

    //-------------------------- Old VisualizerTitleBar fields -------------------------------------------
    QAction *actionSplitVertically;
    QAction *actionSplitHorizontally;
    QAction *actionNone;
    QLabel *label;
    QComboBox *comboType;

    QToolButton *buttonSplitV;
    QToolButton *buttonSplitH;

    //! Bie¿¹cy wizualizator.
    VisualizerPtr visualizer;
    //customowe elementu titlebara dla danego wizualizatora
    std::vector<QObject*> visualizerCustomElements;

    //-------------------------- Old VisualizerTitleBarComboPick fields -------------------------------------------
    QToolButton *buttonSource;

    //! Menu do wyboru Ÿróde³.
    QMenu* menuSource;

    QWidget * visualizerWidget;


    TimelineChannels timelineChannels;

    std::map<core::ObjectWrapperConstPtr, core::VisualizerSeriePtr > currentSeriesData;
    std::map<core::TypeInfo, std::set<core::ObjectWrapperConstPtr> > groupedSeriesData;

    std::pair<QAction*, core::ObjectWrapperConstPtr> lastSerie;

public:
    //! Zeruj¹cy konstruktor.
    //! \param parent
    //! \param flags
    VisualizerWidget(QWidget* parent = nullptr, Qt::WindowFlags flags = 0);
    //!
    VisualizerWidget(UniqueID visualizerID, QWidget* parent = nullptr, Qt::WindowFlags flags = 0);
    //!
    VisualizerWidget(const VisualizerPtr& source, QWidget* parent = nullptr, Qt::WindowFlags flags = 0);
        
            
    //! Zapewnia mo¿liwoœæ kasowanie widgeta wizualizatora przez jego implementacjê.
    ~VisualizerWidget();
        
public:

    //-------------------------- Old VisualizerTitleBar methods -------------------------------------------
    //! Dodaje wizualizator do listy obs³ugiwanych.
    void addVisualizer(const QString& label, UniqueID id);
    //! Dodaje wizualizator do listy obs³ugiwanych.
    void addVisualizer(const QIcon& icon, const QString& label, UniqueID id);

    //! \return Liczba zarejestrowanych wizualizatorów.
    int getNumVisualizers() const;
    //! Usuwa wszystkie wizualizatory.
    void removeAllVisualizers();

    //! Przywraca stan pocz¹tkowy.
    void clearCurrentVisualizer();

    //! \return Bie¿¹cy wizualizator.
    inline VisualizerPtr getCurrentVisualizer()
    {
        return visualizer;
    }

    void setSourceVisible(bool visible);
    bool isSourceVisible() const;

    void setSplitHVisible(bool visible);
    bool isSplitHVisible() const;

    void setSplitVVisible(bool visible);
    bool isSplitVVisible() const;

    void setVisualizerIconVisible(bool visible);
    bool isVisualizerIconVisible() const;

    void setActiveVisualizerSwitch(bool lock);
    bool isActiveVisualizerSwich() const;

protected:
    virtual void closeEvent (QCloseEvent *event);
        
private slots:

    void removeAllSeries();

    //! \param Czy zamieniæ pasek tytu³owy na systemowy?
    //void setReplaceTitleBar(bool replace);
    //! \param Czy zamieniæ pasek tytu³owy na systemowy?
    //void queueReplaceTitleBar(bool replace);
    //!
    void splitHorizontally();
    //!
    void splitVertically();
    //!
    void split(Qt::Orientation orientation);

    //-------------------------- Old VisualizerTitleBar slots -------------------------------------------
    //! \param id Id bie¿¹cego wizualizatora.
    void setCurrentVisualizer(UniqueID id);
    //! \param visualizer Bie¿¹cy wizualizator.
    void setCurrentVisualizer(const VisualizerPtr& visualizer);
    //! \param idx Indeks bie¿¹cego wizualizatora.
    void setCurrentVisualizer(int idx);

private slots:

    //-------------------------- Old VisualizerTitleBarComboPick slots -------------------------------------------
    //! 
    void fillSourcesMenu();
    //!
    void sourceSelected();
        
private:

    void innerRemoveAllSeries();

    void clearDataSeries();

    //-------------------------- Old VisualizerTitleBarComboPick methods -------------------------------------------
    void retranslateUi(QWidget * visualizerWidget);
    //! Inicjalizacja wizualizatora.
    void init();
    //-------------------------- Old VisualizerTitleBarComboPick methods -------------------------------------------
    std::string getLabel( const core::ObjectWrapperConstPtr& object, bool noSource );

    void clearSources();

    void clearCurrentVisualizerWidget();

    void addSourceDefaultAction();
};

class IVisualizerChannel : public timeline::IChannel
{
public:
    virtual ~IVisualizerChannel() {}

    virtual void releaseChannel() = 0;
};

class VisualizerChannel : public IVisualizerChannel
{
public:

    VisualizerChannel(const core::VisualizerTimeSeriePtr & serie, VisualizerWidget * visualizer) : serie(serie), constSerie(serie), visualizer(visualizer) {}

    virtual void releaseChannel()
    {
        serie = core::VisualizerTimeSeriePtr();
        visualizer = nullptr;
    }

    virtual ~VisualizerChannel()
    {
        if(visualizer!= nullptr && visualizer->getCurrentVisualizer()){
            visualizer->getCurrentVisualizer()->removeSerie(serie);
        }
    }

    virtual void setTime(double time)
    {
        serie->setTime(time);
    }

    virtual double getLength() const
    {
        return serie->getLength();
    }

    virtual timeline::IChannel * clone() const
    {
        //TODO
        //mo¿na tutaj wprowadzic zarz¹dzanie klonowaniem serii

        return nullptr;
    }

    const core::VisualizerTimeSeriePtr & getSerie()
    {
        return serie;
    }

    const core::VisualizerTimeSerieConstPtr & getSerie() const
    {
        return constSerie;
    }

    VisualizerWidget * getVisualizer()
    {
        return visualizer;
    }

    const VisualizerWidget * getVisualizer() const
    {
        return visualizer;
    }

private:

    core::VisualizerTimeSeriePtr serie;
    core::VisualizerTimeSerieConstPtr constSerie;
    VisualizerWidget * visualizer;
};


class VisualizerMultiChannel : public IVisualizerChannel
{
public:

    typedef std::map<core::VisualizerTimeSeriePtr, VisualizerWidget *> SeriesWidgets;

public:

    VisualizerMultiChannel( const SeriesWidgets seriesWidgets ) : seriesWidgets(seriesWidgets)
    {
        UTILS_ASSERT((seriesWidgets.empty() == false), "Nie podano ¿adnych serii dla kanalu");

        auto it = seriesWidgets.begin();

        length = it->first->getLength();

        it++;

        for( ; it != seriesWidgets.end(); it++){
            if(length < it->first->getLength()){
                length = it->first->getLength();
            }
        }
    }

    virtual void releaseChannel()
    {
        seriesWidgets.swap(SeriesWidgets());
    }

    virtual ~VisualizerMultiChannel()
    {
        for(auto it = seriesWidgets.begin(); it != seriesWidgets.end(); it++){
            if(it->second->getCurrentVisualizer() != nullptr){
                it->second->getCurrentVisualizer()->removeSerie(it->first);
            }
        }
    }

    virtual void setTime(double time)
    {
        for(auto it = seriesWidgets.begin(); it != seriesWidgets.end(); it++){
            it->first->setTime(time);
        }
    }

    virtual double getLength() const
    {
        return length;
    }

    virtual timeline::IChannel * clone() const
    {
        //TODO
        //mo¿na tutaj wprowadzic zarz¹dzanie klonowaniem serii

        return nullptr;
    }

    const SeriesWidgets & getSeriesWidgets() const
    {
        return seriesWidgets;
    }

private:

    SeriesWidgets seriesWidgets;
    float length;
};

typedef boost::shared_ptr<VisualizerMultiChannel> VisualizerMultiChannelPtr;
typedef boost::shared_ptr<const VisualizerMultiChannel> VisualizerMultiChannelConstPtr;


#endif  // HEADER_GUARD_CORE__VISUALIZERWIDGET_H__