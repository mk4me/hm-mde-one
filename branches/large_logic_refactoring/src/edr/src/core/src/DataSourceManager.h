/********************************************************************
    created:  2011/05/30
    created:  30:5:2011   8:40
    filename: DataSourceManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___DATASOURCEMANAGER_H__
#define HEADER_GUARD___DATASOURCEMANAGER_H__

#include <utils/ObserverPattern.h>
#include <core/IDataSourceManager.h>
#include <core/IDataSource.h>
#include "DataSource.h"
#include "ManagerHelper.h"

namespace core {

class DataSourceManager : public plugin::IDataSourceManager, public utils::Observable<DataSourceManager>, public ManagerHelper<DataSourceManager>
{
    friend class DataSource;
public:
    //! Lista źródeł danych.
    typedef std::vector<plugin::IDataSourcePtr> IDataSources;
    //! 
    typedef boost::iterator_range<IDataSources::const_iterator> IDataSourcesConstRange;
    //!
    typedef std::list<DataSource*> DataSources;

private:

    //! Prototypy źródeł danych.
    IDataSources prototypes;
    //! Lista śledząca źródła danych.
    DataSources dataSources;

public:
    //! Tworzenie i niszczenie tylko przez metody singletonu.
    DataSourceManager();
    //! Tworzenie i niszczenie tylko przez metody singletonu.
    ~DataSourceManager();

public:

    //! \param dataSource
    virtual void registerDataSource(const plugin::IDataSourcePtr & dataSource);

    //! \return Wyliczenie prototypów.
    inline IDataSourcesConstRange enumPrototypes() const
    {
        return boost::make_iterator_range(prototypes.begin(), prototypes.end());
    }
    //! \return Liczba zarejestrowanych prototypów.
    inline int getNumPrototypes() const
    {
        return static_cast<int>(prototypes.size());
    }
    //! \param i Indeks źródła.
    inline plugin::IDataSourceConstPtr getPrototype(int i) const
    {
        UTILS_ASSERT(i < getNumPrototypes());
        return prototypes[i];
    }

    //! \param id ID źródła.
    plugin::IDataSourceConstPtr getPrototype(UniqueID id) const;

    //! Tworzy instancję źródła.
    //! \param id id źródła.
    //! \return Instancja źródła.
    DataSourcePtr createDataSource(UniqueID id);

    //! \return Liczba instancji źródeł danego typu.
    int getNumInstances(UniqueID id);

private:
    //! Tworzy instancję źródła.
    //! \param id id źródła.
    //! \return Instancja źróła.
    DataSourcePtr createDataSource(const plugin::IDataSourceConstPtr& prototype);
    //! 
    DataSourcePtr createDataSource(const DataSource& prototype);

    //! \return Indeks prototypu.
    int getPrototypeIdx(UniqueID id) const;

    void notifyCreated(DataSource* dataSource);
    void notifyDestroyed(DataSource* dataSource);
};

}

#endif  //  HEADER_GUARD___DATASOURCEMANAGER_H__

