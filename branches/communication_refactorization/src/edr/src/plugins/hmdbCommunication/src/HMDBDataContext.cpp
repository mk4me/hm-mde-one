#include "CommunicationPCH.h"
#include "HMDBDataContext.h"
#include "ShallowCopyUtils.h"
#include <plugins/hmdbCommunication/IHMDBStorage.h>
#include <plugins/hmdbCommunication/IHMDBRemoteContext.h>

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

const ShallowCopyPtr HMDBDataContext::createShallowCopy() const
{
	auto st = storage_->transaction();
	return ShallowCopyUtils::extractShallowCopy(userHash_, st.get());
}

const bool HMDBDataContext::shallowCopyExists() const
{
	auto st = storage_->transaction();

	const bool motionExists = ShallowCopyUtils::shallowCopyInStorage(userHash_, IHMDBRemoteContext::Motion, ShallowCopyUtils::Shallow, st.get()) &&
		ShallowCopyUtils::shallowCopyInStorage(userHash_, IHMDBRemoteContext::Motion, ShallowCopyUtils::Meta, st.get());

	const bool medicalExists = ShallowCopyUtils::shallowCopyInStorage(userHash_, IHMDBRemoteContext::Medical, ShallowCopyUtils::Shallow, st.get()) &&
		ShallowCopyUtils::shallowCopyInStorage(userHash_, IHMDBRemoteContext::Medical, ShallowCopyUtils::Meta, st.get());

	return motionExists || medicalExists;		
}