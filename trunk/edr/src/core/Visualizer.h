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

//! Wizualizator. Jego zadaniem jest stworzyÊ widget (gdzie on bÍdzie osadzony - nie jego sprawa),
//! zadeklarowaÊ ile ürÛde≥ i jakiego typu jest w stanie obs≥uøyÊ oraz przyjπÊ ürÛd≥a danych.
class Visualizer : public InputItem<core::IVisualizer>
{
private:
    //! Faktyczny widget.
    QWidget* widget;

    //! èrÛd≥o danych dla wizualizatora.
    //ObjectSource source;

    //! Akcje wizualizatora.
    std::vector<QObject*> genericActions;
    //! Nazwa uzupe≥niona o przyrostek.
    QString uiName;

    std::set<core::shared_ptr<core::IVisualizer::SerieBase> > dataSeries;


public:
    //! \param impl Implementacja wizualizatora. Obiekt przejmowany na w≥asnoúÊ.
    Visualizer( core::IVisualizer* impl );
    //! Konstuktor kopiujπcy - g≥Íboka kopia. Nie kopiuje widgeta.
    Visualizer( const Visualizer& visualizer );
    //! Niepolimorficzny (!) destruktor 
    ~Visualizer();

public:
    //! \return Lista akcji ogÛlnych wizualizatora.
    const std::vector<QObject*>& getGenericActions() const;
    //! \return Lista akcji ogÛlnych wizualizatora.
    const std::vector<QObject*>& getOrCreateGenericActions();

    //! \return Widget wizualizatora.
    QWidget* getOrCreateWidget();
    //! \return Widget wizualizatora. Moøe byÊ nullptr, jeøeli jeszcze nie jest stworzony.
    inline QWidget* getWidget()
    {
        return widget;
    }

    inline void update(double deltaTime)
    {
        getImplementation()->update(deltaTime);
    }

    //! \return Nazwa wizualizatora. Uøywana w UI.
    const QString& getUIName() const;

    //! \param uiName Nazwa wizualizatora uøywana w UI. Nie powoduje automatycznego odúwieøenia UI.
    void setUIName(const QString& uiName);

    //!
    const QIcon& getIcon() const;

    int getMaxSeries() const
    {
        return getImplementation()->getMaxDataSeries();
    }

    const core::VisualizerSeriePtr & createSerie(const core::ObjectWrapperConstPtr & data, const std::string & name)
    {
        core::VisualizerSeriePtr serie(getImplementation()->createSerie(data, name));
        dataSeries.insert(serie);
        return *(dataSeries.find(serie));
    }

    void removeSerie(const core::VisualizerSeriePtr & serie)
    {
        getImplementation()->removeSerie(serie.get());
        dataSeries.erase(serie);
    }
};

typedef core::shared_ptr<Visualizer> VisualizerPtr;
typedef core::shared_ptr<const Visualizer> VisualizerConstPtr;
typedef core::weak_ptr<Visualizer> VisualizerWeakPtr;
typedef core::weak_ptr<const Visualizer> VisualizerConstWeakPtr;

#endif  // HEADER_GUARD_CORE__VISUALIZER_H__