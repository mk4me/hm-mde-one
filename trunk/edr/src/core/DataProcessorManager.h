/********************************************************************
    created:  2011/05/23
    created:  23:5:2011   9:12
    filename: DataProcessorManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___DATAPROCESSORMANAGER_H__
#define HEADER_GUARD___DATAPROCESSORMANAGER_H__

#include <utils/ObserverPattern.h>
#include <core/IDataProcessorManager.h>
#include "DataProcessor.h"

class DataProcessorManager : public core::IDataProcessorManager, public utils::Observable<DataProcessorManager>
{
    friend class DataProcessor;
public:
    //! Lista wizualizator�w.
    typedef std::vector<core::IDataProcessorPtr> IDataProcessors;
    //! 
    typedef boost::iterator_range<IDataProcessors::const_iterator> IDataProcessorsConstRange;
    //!
    //! typedef std::list<VisualizerWeakPtr> WeakVisualizers;
    typedef std::list<DataProcessor*> DataProcessors;
    //! Lista typ�w kolejnych �r�de�.
    //typedef Visualizer::SlotsInfo SourcesTypes; 

private:
    ////! Niezmienne dane pobrane z wizualizator�w.
    //struct IVisualizerPersistantData
    //{
    //    //! Ikona.
    //    QIcon icon;
    //    //! Lista typ�w kolejnych �r�de�.
    //    SourcesTypes sourcesTypes;
    //};

    //! Prototypy wizualizator�w.
    IDataProcessors prototypes;
    //! Lista �ledz�ca wizualizatory.
    // WeakVisualizers visualizersTrace;
    DataProcessors dataProcessors;

public:
    //! Tworzenie i niszczenie tylko przez metody singletonu.
    DataProcessorManager();
    //! Tworzenie i niszczenie tylko przez metody singletonu.
    ~DataProcessorManager();

public:
    static DataProcessorManager* getInstance()
    {
        return static_cast<DataProcessorManager*>(core::getDataProcessorManager());
    }

    //! \param visualizer
    virtual void registerDataProcessor(const core::IDataProcessorPtr & dataProcessor);

    //! Aktualizacja wizualizator�w.
    void update();

    //! \return Ikona dla danego wizualizatora.
    //const QIcon& getIcon(UniqueID id) const;
    //! \return Informacje o wej�ciu dla danego wizualizatora.
    //const SourcesTypes& getSourcesTypes(UniqueID id) const;

    //! \return Wyliczenie prototyp�w.
    inline IDataProcessorsConstRange enumPrototypes() const
    {
        return boost::make_iterator_range(prototypes.begin(), prototypes.end());
    }
    //! \return Liczba zarejestrowanych prototyp�w.
    inline int getNumPrototypes() const
    {
        return static_cast<int>(prototypes.size());
    }
    //! \param i Indeks wizualizatora.
    inline core::IDataProcessorConstPtr getPrototype(int i) const
    {
        UTILS_ASSERT(i < getNumPrototypes());
        return prototypes[i];
    }

    //! \param id ID wizualizatora.
    core::IDataProcessorConstPtr getPrototype(UniqueID id) const;

    //! Tworzy instancj� wizualizatora.
    //! \param id id wizualizatora.
    //! \return Instancja wizualizatora.
    DataProcessorPtr createDataProcessor(UniqueID id);

    //! \return Liczba instancji wizualizator�w danego typu.
    int getNumInstances(UniqueID id);

private:
    //! Tworzy instancj� wizualizatora.
    //! \param id id wizualizatora.
    //! \return Instancja wizualizatora.
    DataProcessorPtr createDataProcessor(const core::IDataProcessorConstPtr& prototype);
    //! 
    DataProcessorPtr createDataProcessor(const DataProcessor& prototype);

private:
    //! \return Indeks prototypu.
    int getPrototypeIdx(UniqueID id) const;

    void notifyCreated(DataProcessor* dataProcessor);
    void notifyDestroyed(DataProcessor* dataProcessor);
};

#endif  //    HEADER_GUARD___DATAPROCESSORMANAGER_H__

