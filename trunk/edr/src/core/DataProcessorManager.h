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
    //! Lista elementów przetwarzaj¹cych.
    typedef std::vector<core::IDataProcessorPtr> IDataProcessors;
    //! 
    typedef boost::iterator_range<IDataProcessors::const_iterator> IDataProcessorsConstRange;
    //!
    typedef std::list<DataProcessor*> DataProcessors;

private:

    //! Prototypy elementów przetwarzaj¹cych.
    IDataProcessors prototypes;
    //! Lista œledz¹ca elementy przwtwarzaj¹ce.
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
    //! \param i Indeks elementu przetwarzajacego.
    inline core::IDataProcessorConstPtr getPrototype(int i) const
    {
        UTILS_ASSERT(i < getNumPrototypes());
        return prototypes[i];
    }

    //! \param id ID elementu przetwarzajacego.
    core::IDataProcessorConstPtr getPrototype(UniqueID id) const;

    //! Tworzy instancjê elementu przetwarzajacego.
    //! \param id id elementu przetwarzajacego.
    //! \return Instancja elementu przetwarzajacego.
    DataProcessorPtr createDataProcessor(UniqueID id);

    //! \return Liczba instancji elementow przetwarzajacych danego typu.
    int getNumInstances(UniqueID id);

private:
    //! Tworzy instancjê elementu przetwarzajacego.
    //! \param id id elementu przetwarzajacego.
    //! \return Instancja elementu przetwarzajacego.
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

