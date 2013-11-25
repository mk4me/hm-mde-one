#include "CoreLibPCH.h"
#include <corelib/AbstractSerie.h>

void plugin::AbstractSerie::setData( const core::TypeInfo & requestedDataType, const core::ObjectWrapperConstPtr & data )
{
    this->requestedType = requestedDataType;
    this->data = data;
    setupData(data);
}

const core::TypeInfo & plugin::AbstractSerie::getRequestedDataType() const
{
    return requestedType;
}

const core::ObjectWrapperConstPtr & plugin::AbstractSerie::getData() const
{
    return data;
}

const std::string plugin::AbstractSerie::getName() const
{
    return name;
}

void plugin::AbstractSerie::setName( const std::string & name )
{
    this->name = name;
}
