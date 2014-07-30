#include "CommunicationPCH.h"
#include <plugins/hmdbCommunication/DataStatus.h>

using namespace hmdbCommunication;

const DataStatus DataStatus::PartialStatus = DataStatus(PartiallyLocal, PartiallyLoaded, PartiallyValid);

DataStatus::DataStatus(const DataStatus & status) : storage_(status.storage_),
usage_(status.usage_), validity_(status.validity_)
{

}

DataStatus::DataStatus(const DataStorage storage, const DataUsage usage,
	const DataValidity validity) : storage_(storage), usage_(usage),
	validity_(validity)
{

}

DataStatus::DataStatus(const DataStorage storage)
	: storage_(storage), usage_(UnknownUsage),
	validity_(UnknownValidity)
{

}

DataStatus::DataStatus(const DataUsage usage)
	: storage_(UnknownStorage), usage_(usage),
	validity_(UnknownValidity)
{

}

DataStatus::DataStatus(const DataValidity validity)
	: storage_(UnknownStorage), usage_(UnknownUsage),
	validity_(validity)
{

}

DataStatus::~DataStatus()
{

}

const DataStorage DataStatus::storage() const
{
	return storage_;
}

const DataUsage DataStatus::usage() const
{
	return usage_;
}

const DataValidity DataStatus::validity() const
{
	return validity_;
}

const DataStorage DataStatus::merge(const DataStorage storage)
{
	storage_ |= storage;
	return storage_;
}

const DataUsage DataStatus::merge(const DataUsage usage)
{
	usage_ |= usage;
	return usage_;
}

const DataValidity DataStatus::merge(const DataValidity validity)
{
	validity_ |= validity;
	return validity_;
}

const DataStatus & DataStatus::merge(const DataStatus & status)
{
	merge(status.storage());
	merge(status.usage());
	merge(status.validity());
	return *this;
}

/*
DataStatus::operator DataStorage()
{
	return storage();
}

DataStatus::operator DataUsage()
{
	return usage();
}

DataStatus::operator DataValidity()
{
	return validity();
}
*/

bool operator==(const DataStatus & a, const DataStatus & b)
{
	return ((a.storage() == b.storage())
		&& (a.usage() == b.usage())
		&& (a.validity() == b.validity()));
}

bool operator!=(const DataStatus & a, const DataStatus & b)
{
	return !(a == b);
}

bool DataStatus::operator<(const DataStatus & b) const
{
	bool ret = true;

	if (storage() == b.storage()){
		if (usage() == b.usage()){
			if (validity() >= b.validity()){
				ret = false;
			}
		}
		else if (usage() > b.usage()){
			ret = false;
		}
	}
	else if (storage() > b.storage()){
		ret = false;
	}

	return ret;
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

DataValidity operator|(DataValidity l, DataValidity r)
{
	return static_cast<DataValidity>((int)l | (int)r);
}

DataValidity & operator|=(DataValidity & l, DataValidity r)
{
	return l = (l | r);
}

DataStatus operator|(const DataStatus & l, const DataStatus & r)
{
	return DataStatus(l.storage() | r.storage(), l.usage() | r.usage(),
		l.validity() | r.validity());
}

DataStatus operator|(const DataStatus & l, const DataStorage & r)
{
	DataStatus ds(l);
	ds.merge(r);
	return ds;
}

DataStatus & operator|=(DataStatus & l, const DataStorage & r)
{
	l.merge(r);
	return l;
}

DataStatus operator|(const DataStatus & l, const DataUsage & r)
{
	DataStatus ds(l);
	ds.merge(r);
	return ds;
}

DataStatus & operator|=(DataStatus & l, const DataUsage & r)
{
	l.merge(r);
	return l;
}

DataStatus operator|(const DataStatus & l, const DataValidity & r)
{
	DataStatus ds(l);
	ds.merge(r);
	return ds;
}

DataStatus & operator|=(DataStatus & l, const DataValidity & r)
{
	l.merge(r);
	return l;
}

DataStatus & operator|=(DataStatus & l, const DataStatus & r)
{
	l.merge(r);
	return l;
}
