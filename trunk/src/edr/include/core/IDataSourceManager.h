/********************************************************************
    created:  2011/05/30
    created:  30:5:2011   8:53
    filename: IDataSourceManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IDATASOURCEMANAGER_H__
#define HEADER_GUARD___IDATASOURCEMANAGER_H__

#include <core/IDataSource.h>
#include <utils/Debug.h>

namespace core{

    class IDataSourceManager
    {
    public:
        virtual ~IDataSourceManager(void) {};

    };

    typedef shared_ptr<IDataSourceManager> IDataSourceManagerPtr;
    typedef shared_ptr<const IDataSourceManager> IDataSourceManagerConstPtr;
    typedef weak_ptr<IDataSourceManager> IDataSourceManagerWeakPtr;
    typedef weak_ptr<const IDataSourceManager> IDataSourceManagerWeakConstPtr;

    ////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////


#endif  //  HEADER_GUARD___IDATASOURCEMANAGER_H__
