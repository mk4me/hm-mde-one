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
#include <typeinfo>
#include <boost/any.hpp>
#include <boost/range.hpp>
#include <boost/iterator.hpp>
#include <vector>
#include <core/ObjectWrapper.h>
#include "Visualizer.h"
#include <QtCore/QMetaType>

Q_DECLARE_METATYPE(UniqueID);

class DataManager;
class VisualizerManager
{
public:
    //! Lista wizualizator�w.
    typedef std::vector<core::IVisualizerPtr> IVisualizers;
    //! 
    typedef boost::iterator_range<IVisualizers::const_iterator> IVisualizersConstRange;
    //!
    typedef std::list<VisualizerWeakPtr> WeakVisualizers;
    //! Lista typ�w kolejnych �r�de�.
    typedef Visualizer::SourcesTypes SourcesTypes; 

private:
    //! Niezmienne dane pobrane z wizualizator�w.
    struct IVisualizerPersistantData
    {
        //! Ikona.
        QIcon icon;
        //! Lista typ�w kolejnych �r�de�.
        SourcesTypes sourcesTypes;
    };


    //! Instancja wizualizatora.
    static VisualizerManager* instance;
    //! Prototypy wizualizator�w.
    IVisualizers prototypes;
    //! Lista �ledz�ca wizualizatory.
    WeakVisualizers visualizersTrace;
    //! Sta�e dane wizualizator�w.
    std::vector< IVisualizerPersistantData* > visualizersData;

private:
    //! Tworzenie i niszczenie tylko przez metody singletonu.
    VisualizerManager() {}
    //! Tworzenie i niszczenie tylko przez metody singletonu.
    ~VisualizerManager();

public:
    //! \return Instancja DataManagera.
    inline static VisualizerManager* getInstance()
    { 
        return instance;
    }
    //! Tworzy instancj� DataManagera.
    static void createInstance();
    //! Niszczy instancj� DataManagera.
    static void destroyInstance();

public:
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
    //! Tworzy instancj� wizualizatora.
    //! \param id id wizualizatora.
    //! \return Instancja wizualizatora.
    VisualizerPtr createVisualizer(const core::IVisualizerConstPtr& prototype);
    //! 
    VisualizerPtr createVisualizer(const Visualizer& prototype);

private:
    //! \return Indeks prototypu.
    int getPrototypeIdx(UniqueID id) const;
};



#endif  // HEADER_GUARD_CORE__VISUALIZERMANAGER_H__