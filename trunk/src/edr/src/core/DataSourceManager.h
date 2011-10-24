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
#include "DataSource.h"
#include "ManagerHelper.h"

class DataSourceManager : public core::IDataSourceManager, public utils::Observable<DataSourceManager>, public ManagerHelper<DataSourceManager>
{
    friend class DataSource;
public:
    //! Lista �r�de� danych.
    typedef std::vector<core::IDataSourcePtr> IDataSources;
    //! 
    typedef boost::iterator_range<IDataSources::const_iterator> IDataSourcesConstRange;
    //!
    typedef std::list<DataSource*> DataSources;

private:

    //! Prototypy �r�de� danych.
    IDataSources prototypes;
    //! Lista �ledz�ca �r�d�a danych.
    DataSources dataSources;

public:
    //! Tworzenie i niszczenie tylko przez metody singletonu.
    DataSourceManager();
    //! Tworzenie i niszczenie tylko przez metody singletonu.
    ~DataSourceManager();

public:

    //! \param dataSource
    virtual void registerDataSource(const core::IDataSourcePtr & dataSource);

    //! \return Wyliczenie prototyp�w.
    inline IDataSourcesConstRange enumPrototypes() const
    {
        return boost::make_iterator_range(prototypes.begin(), prototypes.end());
    }
    //! \return Liczba zarejestrowanych prototyp�w.
    inline int getNumPrototypes() const
    {
        return static_cast<int>(prototypes.size());
    }
    //! \param i Indeks �r�d�a.
    inline core::IDataSourceConstPtr getPrototype(int i) const
    {
        UTILS_ASSERT(i < getNumPrototypes());
        return prototypes[i];
    }

    //! \param id ID �r�d�a.
    core::IDataSourceConstPtr getPrototype(UniqueID id) const;

    //! Tworzy instancj� �r�d�a.
    //! \param id id �r�d�a.
    //! \return Instancja �r�d�a.
    DataSourcePtr createDataSource(UniqueID id);

    //! \return Liczba instancji �r�de� danego typu.
    int getNumInstances(UniqueID id);

private:
    //! Tworzy instancj� �r�d�a.
    //! \param id id �r�d�a.
    //! \return Instancja �r�a.
    DataSourcePtr createDataSource(const core::IDataSourceConstPtr& prototype);
    //! 
    DataSourcePtr createDataSource(const DataSource& prototype);

    //! \return Indeks prototypu.
    int getPrototypeIdx(UniqueID id) const;

    void notifyCreated(DataSource* dataSource);
    void notifyDestroyed(DataSource* dataSource);
};

#endif  //  HEADER_GUARD___DATASOURCEMANAGER_H__

