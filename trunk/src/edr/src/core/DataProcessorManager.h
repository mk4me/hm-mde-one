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
#include "ManagerHelper.h"

class DataProcessorManager : public core::IDataProcessorManager, public utils::Observable<DataProcessorManager>, public ManagerHelper<DataProcessorManager>
{
    friend class DataProcessor;
public:
    //! Lista elementów przetwarzających.
    typedef std::vector<core::IDataProcessorPtr> IDataProcessors;
    //! 
    typedef boost::iterator_range<IDataProcessors::const_iterator> IDataProcessorsConstRange;
    //!
    typedef std::list<DataProcessor*> DataProcessors;

private:

    //! Prototypy elementów przetwarzających.
    IDataProcessors prototypes;
    //! Lista śledząca elementy przwtwarzające.
    DataProcessors dataProcessors;

public:
    //! Tworzenie i niszczenie tylko przez metody singletonu.
    DataProcessorManager();
    //! Tworzenie i niszczenie tylko przez metody singletonu.
    ~DataProcessorManager();

public:

    //! \param dataProcessor
    virtual void registerDataProcessor(const core::IDataProcessorPtr & dataProcessor);

    //! \return Wyliczenie prototypów.
    inline IDataProcessorsConstRange enumPrototypes() const
    {
        return boost::make_iterator_range(prototypes.begin(), prototypes.end());
    }
    //! \return Liczba zarejestrowanych prototypów.
    inline int getNumPrototypes() const
    {
        return static_cast<int>(prototypes.size());
    }
    //! \param i Indeks elementu przetwarzającego.
    inline core::IDataProcessorConstPtr getPrototype(int i) const
    {
        UTILS_ASSERT(i < getNumPrototypes());
        return prototypes[i];
    }

    //! \param id ID elementu przetwarzającego.
    core::IDataProcessorConstPtr getPrototype(UniqueID id) const;

    //! Tworzy instancję elementu przetwarzającego.
    //! \param id id elementu przetwarzającego.
    //! \return Instancja elementu przetwarzającego.
    DataProcessorPtr createDataProcessor(UniqueID id);

    //! \return Liczba instancji elementów przetwarzających danego typu.
    int getNumInstances(UniqueID id);

private:
    //! Tworzy instancję elementu przetwarzającego.
    //! \param id id elementu przetwarzającego.
    //! \return Instancja elementu przetwarzającego.
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

