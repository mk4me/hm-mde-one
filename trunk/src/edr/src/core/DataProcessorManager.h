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
    //! \param i Indeks elementu przetwarzaj¹cego.
    inline core::IDataProcessorConstPtr getPrototype(int i) const
    {
        UTILS_ASSERT(i < getNumPrototypes());
        return prototypes[i];
    }

    //! \param id ID elementu przetwarzaj¹cego.
    core::IDataProcessorConstPtr getPrototype(UniqueID id) const;

    //! Tworzy instancjê elementu przetwarzaj¹cego.
    //! \param id id elementu przetwarzaj¹cego.
    //! \return Instancja elementu przetwarzaj¹cego.
    DataProcessorPtr createDataProcessor(UniqueID id);

    //! \return Liczba instancji elementów przetwarzaj¹cych danego typu.
    int getNumInstances(UniqueID id);

private:
    //! Tworzy instancjê elementu przetwarzaj¹cego.
    //! \param id id elementu przetwarzaj¹cego.
    //! \return Instancja elementu przetwarzaj¹cego.
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

