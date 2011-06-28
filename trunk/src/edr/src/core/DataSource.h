/********************************************************************
    created:  2011/05/30
    created:  30:5:2011   8:25
    filename: DataSource.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___DATASOURCE_H__
#define HEADER_GUARD___DATASOURCE_H__

#include "OutputItem.h"
#include <core/IDataSource.h>

class DataSource : public OutputItem<core::IDataSource>
{
public:
    DataSource(core::IDataSource * impl);
    ~DataSource();
};

typedef core::shared_ptr<DataSource> DataSourcePtr;
typedef core::shared_ptr<const DataSource> DataSourceConstPtr;


#endif  //  HEADER_GUARD___DATASOURCE_H__