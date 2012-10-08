#include "CommunicationPCH.h"
#include "DataSourceStatus.h"

using namespace communication;

DataStatus::DataStatus(const DataStatus & status) : storage_(status.storage_), usage_(status.usage_)
{

}

DataStatus::DataStatus(DataStorage storage, DataUsage usage) : storage_(storage), usage_(usage)
{

}

DataStatus::~DataStatus()
{

}

void DataStatus::setStorage(DataStorage storage)
{
    storage_ = storage;
}

const DataStorage DataStatus::storage() const
{
    return storage_;
}

void DataStatus::setUsage(DataUsage usage)
{
    usage_ = usage;
}

const DataUsage DataStatus::usage() const
{
    return usage_;
}

DataStatus & DataStatus::operator=(const DataStatus & status)
{
    storage_ = status.storage_;
    usage_ = status.usage_;
    return *this;
}

bool operator==(const DataStatus & a, const DataStatus & b)
{
    if(a.storage_ == b.storage_ && a.usage_ == b.usage_){
        return true;
    }

    return false;
}

bool operator!=(const DataStatus & a, const DataStatus & b)
{
    return !(a == b);
}

DataStorage operator|(DataStorage l, DataStorage r)
{
    return static_cast<DataStorage>((int)l | (int)r);
}

DataStorage & operator|=(DataStorage & l, DataStorage r)
{
    return l = (l | r);
}

DataUsage operator|(DataUsage l, DataUsage r)
{
    return static_cast<DataUsage>((int)l | (int)r);
}

DataUsage & operator|=(DataUsage & l, DataUsage r)
{
    return l = (l | r);
}

DataStatus operator|(const DataStatus & l, const DataStatus & r)
{
    return DataStatus(l.storage() | r.storage(), l.usage() | r.usage());
}

DataStatus & operator|=(DataStatus & l, const DataStatus & r)
{
    l.setStorage(l.storage() | r.storage());
    l.setUsage(l.usage() | r.usage());
    return l;
}
