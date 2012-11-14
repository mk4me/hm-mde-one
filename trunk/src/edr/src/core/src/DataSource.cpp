#include "CorePCH.h"
#include "DataSource.h"

DataSource::DataSource(core::IDataSource * impl)
    : OutputItem<core::IDataSource>(impl)
{

}

DataSource::~DataSource()
{

}
