/********************************************************************
    created:  2011/05/30
    created:  30:5:2011   8:53
    filename: IDataSourceManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IDATASOURCEMANAGER_H__
#define HEADER_GUARD___IDATASOURCEMANAGER_H__

#include <plugins/newVdf/IDataSource.h>
#include <utils/Debug.h>
#include <utils/ObserverPattern.h>

namespace vdf {

    class IDataSourceManager : public utils::Observable<IDataSourceManager>
    {
    public:
        //! Lista Ÿróde³ danych.
        typedef std::vector<vdf::IDataSourcePtr> IDataSources;
        //! 
        typedef boost::iterator_range<IDataSources::const_iterator> IDataSourcesConstRange;
    public:
        virtual ~IDataSourceManager(void) {};

    public:
        virtual IDataSourcesConstRange enumPrototypes() const = 0;

    };

    typedef core::shared_ptr<IDataSourceManager> IDataSourceManagerPtr;
    typedef core::shared_ptr<const IDataSourceManager> IDataSourceManagerConstPtr;
    typedef core::weak_ptr<IDataSourceManager> IDataSourceManagerWeakPtr;
    typedef core::weak_ptr<const IDataSourceManager> IDataSourceManagerWeakConstPtr;

    ////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////


#endif  //  HEADER_GUARD___IDATASOURCEMANAGER_H__
