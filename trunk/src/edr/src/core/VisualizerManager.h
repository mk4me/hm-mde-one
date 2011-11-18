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
#include <QtGui/QIcon>
#include <core/IVisualizerManager.h>
#include <core/IManagersAccessor.h>
#include "ManagerHelper.h"

Q_DECLARE_METATYPE(UniqueID);

class SceneGraphWidget;
class DataManager;

class VisualizerManager : public core::IVisualizerManager, public utils::Observable<VisualizerManager>, public ManagerHelper<VisualizerManager>
{
    friend class Visualizer;
    friend class IVisualizerChannel;
public:
    //! Lista wizualizator�w.
    typedef std::vector<core::IVisualizerPtr> IVisualizers;
    //! 
    typedef boost::iterator_range<IVisualizers::const_iterator> IVisualizersConstRange;

    typedef std::list<Visualizer*> Visualizers;
    typedef std::list<IVisualizerChannel*> VisualizerChannels;

    typedef std::map<Visualizer*, std::list<IVisualizerChannel*>> GroupedVisualizerChannels;

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

	//! pomocnicza mapa (typ obiektu domenowego) -> (identyfikator wizualizatora)
	std::map<core::TypeInfo, UniqueID> mapType2ID;

    //! Prototypy wizualizator�w.
    IVisualizers prototypes;

    Visualizers visualizers;

    VisualizerChannels visualizerChannels;
    GroupedVisualizerChannels groupedVisualizerChannels;

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

    //! Sprawdzenie czy dla danego typu danych istnieje wizualizator
    //! \param type Typ danych dla kt�rego chcemy uzyska� wizualizator
    //! \return Instancja wizualizatora lub wyj�tek je�li nie mo�na utworzy� (bo nie istnieje + inne b��dy)
    inline bool existVisualizerForType(const core::TypeInfo & type) const
    {
        DataManager* dataManager = DataManager::getInstance();
        for (auto it = mapType2ID.begin(); it != mapType2ID.end(); it++) {
            if (dataManager->isTypeCompatible(type, it->first)) {
                return true;
            }
        }

        return mapType2ID.find(type) != mapType2ID.end();
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

// TODO: create publiczne bez friend Visualizer czy prywatne + jakis inny mechanizm dostepu?
public:
	//! Tworzy instancj� wizualizatora.
	//! \param typeInfo typ, ktory bedzie wyswietlany w wizualizatorze.
	//! \return Instancja wizualizatora.
	VisualizerPtr createVisualizer(const core::TypeInfo& typeInfo);

    void clearVisualizerChannels(Visualizer * visualizer);

private:
    //! \return Indeks prototypu.
    int getPrototypeIdx(UniqueID id) const;

    void notifyCreated(Visualizer* visualizer);
    void notifyDestroyed(Visualizer* visualizer);

    void notifyCreated(IVisualizerChannel* channel);
    void notifyDestroyed(IVisualizerChannel* channel);
};



#endif  // HEADER_GUARD_CORE__VISUALIZERMANAGER_H__