#include "CommunicationPCH.h"
#include "HMDBDataContext.h"

using namespace hmdbCommunication;

HMDBDataContext::HMDBDataContext(IHMDBStorage * storage,
	const std::string & userName,
	const std::string & userHash)
	: storage_(storage), userName_(userName),
	userHash_(userHash)
{

}

HMDBDataContext::~HMDBDataContext()
{

}

const IHMDBStorage * HMDBDataContext::storage() const
{
	return storage_;
}

IHMDBStorage * HMDBDataContext::storage()
{
	return storage_;
}

const std::string HMDBDataContext::userName() const
{
	return userName_;
}

const std::string HMDBDataContext::userHash() const
{
	return userHash_;
}