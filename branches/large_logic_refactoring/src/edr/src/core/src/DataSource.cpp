#include "CorePCH.h"
#include "DataSource.h"

namespace core {

DataSource::DataSource(plugin::IDataSource * impl)
    : OutputItem<plugin::IDataSource>(impl)
{

}

DataSource::~DataSource()
{

}

}