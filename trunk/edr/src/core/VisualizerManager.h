/********************************************************************
	created:  2011/03/11
	created:  11:3:2011   14:29
	filename: VisualizerManager.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__VISUALIZERMANAGER_H__
#define HEADER_GUARD_CORE__VISUALIZERMANAGER_H__

#include <list>
#include <boost/any.hpp>
#include <boost/range.hpp>
#include <boost/iterator.hpp>
#include <vector>
#include <utils/ObserverPattern.h>
#include <core/ObjectWrapper.h>
#include "Visualizer.h"
#include <QtCore/QMetaType>
#include <core/IVisualizerManager.h>

Q_DECLARE_METATYPE(UniqueID);

class SceneGraphWidget;
class DataManager;

class VisualizerManager : public core::IVisualizerManager, public utils::Observable<VisualizerManager>
{
    friend class Visualizer;
public:
    //! Lista wizualizator�w.
    typedef std::vector<core::IVisualizerPtr> IVisualizers;
    //! 
    typedef boost::iterator_range<IVisualizers::const_iterator> IVisualizersConstRange;

    typedef std::list<Visualizer*> Visualizers;
    //! Lista typ�w kolejnych �r�de�.
    typedef ObjectSlots::SlotsInfo SourcesTypes; 

private:
    //! Niezmienne dane pobrane z wizualizator�w.
    struct IVisualizerPersistantData
    {
        //! Ikona.
        QIcon icon;
        //! Lista typ�w kolejnych �r�de�.
        SourcesTypes sourcesTypes;
    };

    //! Prototypy wizualizator�w.
    IVisualizers prototypes;

    Visualizers visualizers;
    //! Sta�e dane wizualizator�w.
    std::vector< IVisualizerPersistantData* > visualizersData;
    //! Widget do wizualizacji struktury sceny 3D. Do debuggowania.
    SceneGraphWidget* debugWidget;

public:
    //! Tworzenie i niszczenie tylko przez metody singletonu.
    VisualizerManager();
    //! Tworzenie i niszczenie tylko przez metody singletonu.
    ~VisualizerManager();

public:
    static VisualizerManager* getInstance()
    {
        return static_cast<VisualizerManager*>(core::getVisualizerManager());
    }

    //! \return
    SceneGraphWidget* getDebugWidget() const
    { 
        return debugWidget;
    }
    //! \param debugWidget
    void setDebugWidget(SceneGraphWidget* debugWidget) 
    { 
        this->debugWidget = debugWidget; 
    }

    //! \param visualizer
    void registerVisualizer(core::IVisualizerPtr visualizer);

    //! Aktualizacja wizualizator�w.
    void update();

    //! \return Ikona dla danego wizualizatora.
    const QIcon& getIcon(UniqueID id) const;
    //! \return Informacje o wej�ciu dla danego wizualizatora.
    const SourcesTypes& getSourcesTypes(UniqueID id) const;

    //! \return Wyliczenie prototyp�w.
    inline IVisualizersConstRange enumPrototypes() const
    {
        return boost::make_iterator_range(prototypes.begin(), prototypes.end());
    }
    //! \return Liczba zarejestrowanych prototyp�w.
    inline int getNumPrototypes() const
    {
        return static_cast<int>(prototypes.size());
    }
    //! \param i Indeks wizualizatora.
    inline core::IVisualizerConstPtr getPrototype(int i) const
    {
        UTILS_ASSERT(i < getNumPrototypes());
        return prototypes[i];
    }

    //! \param id ID wizualizatora.
    core::IVisualizerConstPtr getPrototype(UniqueID id) const;

    //! Tworzy instancj� wizualizatora.
    //! \param id id wizualizatora.
    //! \return Instancja wizualizatora.
    VisualizerPtr createVisualizer(UniqueID id);

    //! \return Liczba instancji wizualizator�w danego typu.
    int getNumInstances(UniqueID id);

private:
    //! Tworzy instancj� wizualizatora.
    //! \param id id wizualizatora.
    //! \return Instancja wizualizatora.
    VisualizerPtr createVisualizer(const core::IVisualizerConstPtr& prototype);
    //! 
    VisualizerPtr createVisualizer(const Visualizer& prototype);

private:
    //! \return Indeks prototypu.
    int getPrototypeIdx(UniqueID id) const;

    void notifyCreated(Visualizer* visualizer);
    void notifyDestroyed(Visualizer* visualizer);
};



#endif  // HEADER_GUARD_CORE__VISUALIZERMANAGER_H__