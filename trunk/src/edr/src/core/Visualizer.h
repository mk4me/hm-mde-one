/********************************************************************
    created:  2011/03/14
    created:  14:3:2011   15:08
    filename: Visualizer.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__VISUALIZER_H__
#define HEADER_GUARD_CORE__VISUALIZER_H__

#include <core/SmartPtr.h>
#include <core/IVisualizer.h>
#include <boost/range.hpp>
#include "InputItem.h"
#include "ObjectSource.h"
#include <timelinelib/IChannel.h>

//! Wizualizator. Jego zadaniem jest stworzyæ widget (gdzie on bêdzie osadzony - nie jego sprawa),
//! zadeklarowaæ ile Ÿróde³ i jakiego typu jest w stanie obs³u¿yæ oraz przyj¹æ Ÿród³a danych.
class Visualizer : public QObject, public InputItem<core::IVisualizer>
{
        Q_OBJECT;
public:
    typedef std::set<core::shared_ptr<core::IVisualizer::SerieBase> > DataSeries;

private:
    //! Faktyczny widget.
    QWidget* widget;

    //! Akcje wizualizatora.
    std::vector<QObject*> genericActions;
    //! Nazwa uzupe³niona o przyrostek.
    QString uiName;

    //! Serie danych utrworzone przez u¿ytkownika
    DataSeries dataSeries;

    ////! Serie danych o charakterze czasowym wystêpuj¹ce w timeline
    //DataSeries timelineDataSeries;

public:
    //! \param impl Implementacja wizualizatora. Obiekt przejmowany na w³asnoœæ.
    Visualizer( core::IVisualizer* impl );
    //! Konstuktor kopiuj¹cy - g³êboka kopia. Nie kopiuje widgeta.
    Visualizer( const Visualizer& visualizer );
    //! Niepolimorficzny (!) destruktor 
    ~Visualizer();

public:
    //! \return Lista akcji ogólnych wizualizatora.
    const std::vector<QObject*>& getGenericActions() const;
    //! \return Lista akcji ogólnych wizualizatora.
    const std::vector<QObject*>& getOrCreateGenericActions();

    //! \return Widget wizualizatora.
    QWidget* getOrCreateWidget();
    //! \return Widget wizualizatora. Mo¿e byæ nullptr, je¿eli jeszcze nie jest stworzony.
    QWidget* getWidget();

    void update(double deltaTime);

    //! \return Nazwa wizualizatora. U¿ywana w UI.
    const QString& getUIName() const;

    //! \param uiName Nazwa wizualizatora u¿ywana w UI. Nie powoduje automatycznego odœwie¿enia UI.
    void setUIName(const QString& uiName);

    //!
    const QIcon& getIcon() const;

    int getMaxSeries() const;

    const DataSeries & getDataSeries() const;

    const core::VisualizerSeriePtr & createSerie(const core::ObjectWrapperConstPtr & data, const std::string & name);

    const core::VisualizerSeriePtr & createSerie(const core::VisualizerSerieConstPtr & serie);

    void removeSerie(const core::VisualizerSeriePtr & serie);

    void clearAllSeries();

    virtual void reset();

signals:
    void printTriggered(const QPixmap& pixmap);

private slots:
    void printActionPressed();
};

typedef core::shared_ptr<Visualizer> VisualizerPtr;
typedef core::shared_ptr<const Visualizer> VisualizerConstPtr;
typedef core::weak_ptr<Visualizer> VisualizerWeakPtr;
typedef core::weak_ptr<const Visualizer> VisualizerConstWeakPtr;

class VisualizerWidget;

class IVisualizerChannel : public timeline::IChannel
{

friend class VisualizerManager;

public:
    IVisualizerChannel(const std::string & path, Visualizer * visualizer);
    virtual ~IVisualizerChannel();

    const std::string & getChannelPath() const;
    const Visualizer * getVisualizer() const;
    Visualizer * getVisualizer();

    //virtual void releaseChannel() = 0;

private:
    std::string path;
    Visualizer * visualizer;
    bool managed;
};

class VisualizerChannel : public IVisualizerChannel
{
public:

    VisualizerChannel(const std::string & path, Visualizer * visualizer, const core::VisualizerTimeSeriePtr & serie);

    //virtual void releaseChannel();

    virtual ~VisualizerChannel();

    //! \return Sklonowany kana³
    virtual VisualizerChannel * clone() const;

    //! \return Dlugosc kanalu w sekundach
    virtual double getLength() const;

    //! Czas zawiera siê miêdzy 0 a getLength()
    //! \param time Aktualny, lokalny czas kanalu w sekundach
    virtual void setTime(double time);

    const core::VisualizerTimeSeriePtr & getSerie();

    const core::VisualizerTimeSerieConstPtr & getSerie() const;

private:

    core::VisualizerTimeSeriePtr serie;
    core::VisualizerTimeSerieConstPtr constSerie;
};


class VisualizerMultiChannel : public IVisualizerChannel
{
public:

    typedef std::vector<core::VisualizerTimeSeriePtr> SeriesWidgets;

public:

    VisualizerMultiChannel(const std::string & path, Visualizer * visualizer, const SeriesWidgets seriesWidgets);

    //virtual void releaseChannel();

    virtual ~VisualizerMultiChannel();

    //! \return Sklonowany kana³
    virtual VisualizerMultiChannel * clone() const;

    //! \return Dlugosc kanalu w sekundach
    virtual double getLength() const;

    //! Czas zawiera siê miêdzy 0 a getLength()
    //! \param time Aktualny, lokalny czas kanalu w sekundach
    virtual void setTime(double time);

    const SeriesWidgets & getSeriesWidgets() const;

private:

    SeriesWidgets seriesWidgets;
    float length;
};

typedef boost::shared_ptr<IVisualizerChannel> IVisualizerChannelPtr;
typedef boost::shared_ptr<const IVisualizerChannel> IVisualizerChannelConstPtr;

typedef boost::shared_ptr<VisualizerChannel> VisualizerChannelPtr;
typedef boost::shared_ptr<const VisualizerChannel> VisualizerChannelConstPtr;

typedef boost::shared_ptr<VisualizerMultiChannel> VisualizerMultiChannelPtr;
typedef boost::shared_ptr<const VisualizerMultiChannel> VisualizerMultiChannelConstPtr;

#endif  // HEADER_GUARD_CORE__VISUALIZER_H__