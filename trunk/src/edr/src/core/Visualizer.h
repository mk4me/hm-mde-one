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

//! Wizualizator. Jego zadaniem jest stworzy� widget (gdzie on b�dzie osadzony - nie jego sprawa),
//! zadeklarowa� ile �r�de� i jakiego typu jest w stanie obs�u�y� oraz przyj�� �r�d�a danych.
class Visualizer : public InputItem<core::IVisualizer>
{
public:
    typedef std::set<core::shared_ptr<core::IVisualizer::SerieBase> > DataSeries;

private:
    //! Faktyczny widget.
    QWidget* widget;

    //! Akcje wizualizatora.
    std::vector<QObject*> genericActions;
    //! Nazwa uzupe�niona o przyrostek.
    QString uiName;

    //! Serie danych utrworzone przez u�ytkownika
    DataSeries dataSeries;

    //! Serie danych o charakterze czasowym wyst�puj�ce w timeline
    DataSeries timelineDataSeries;

public:
    //! \param impl Implementacja wizualizatora. Obiekt przejmowany na w�asno��.
    Visualizer( core::IVisualizer* impl );
    //! Konstuktor kopiuj�cy - g��boka kopia. Nie kopiuje widgeta.
    Visualizer( const Visualizer& visualizer );
    //! Niepolimorficzny (!) destruktor 
    ~Visualizer();

public:
    //! \return Lista akcji og�lnych wizualizatora.
    const std::vector<QObject*>& getGenericActions() const;
    //! \return Lista akcji og�lnych wizualizatora.
    const std::vector<QObject*>& getOrCreateGenericActions();

    //! \return Widget wizualizatora.
    QWidget* getOrCreateWidget();
    //! \return Widget wizualizatora. Mo�e by� nullptr, je�eli jeszcze nie jest stworzony.
    QWidget* getWidget();

    void update(double deltaTime);

    //! \return Nazwa wizualizatora. U�ywana w UI.
    const QString& getUIName() const;

    //! \param uiName Nazwa wizualizatora u�ywana w UI. Nie powoduje automatycznego od�wie�enia UI.
    void setUIName(const QString& uiName);

    //!
    const QIcon& getIcon() const;

    int getMaxSeries() const;

    const DataSeries & getDataSeries() const;

    const DataSeries & getTimelineDataSeries() const;

    const core::VisualizerSeriePtr & createSerie(const core::ObjectWrapperConstPtr & data, const std::string & name);

    void removeSerie(const core::VisualizerSeriePtr & serie);

    void clearAllSeries();

    virtual void reset();
};

typedef core::shared_ptr<Visualizer> VisualizerPtr;
typedef core::shared_ptr<const Visualizer> VisualizerConstPtr;
typedef core::weak_ptr<Visualizer> VisualizerWeakPtr;
typedef core::weak_ptr<const Visualizer> VisualizerConstWeakPtr;

#endif  // HEADER_GUARD_CORE__VISUALIZER_H__