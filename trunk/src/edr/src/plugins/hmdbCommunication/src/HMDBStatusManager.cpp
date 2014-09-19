#include "CommunicationPCH.h"
#include "HMDBStatusManager.h"
#include <corelib/PluginCommon.h>
#include <corelib/IStreamManagerReader.h>
#include <plugins/hmdbCommunication/IHMDBStorage.h>

using namespace hmdbCommunication;

class HMDBStatusManager::HMDBStatusManagerTransaction : public IHMDBStatusManagerOperations
{
public:

	HMDBStatusManagerTransaction(HMDBStatusManager * manager)
		: manager(manager), lock(manager->synch_)
	{
		
	}

	virtual ~HMDBStatusManagerTransaction()
	{

	}
	
	virtual const DataStatus dataStatus(const DataType dataType,
		const hmdbServices::ID id) const
	{
		return manager->rawDataStatus(dataType, id);
	}
	
	virtual void setDataStatus(const DataType dataType,
		const hmdbServices::ID id, const DataStatus & dataStatus)
	{
		manager->rawSetDataStatus(dataType, id, dataStatus);
	}

	virtual void updateDataStatus(const DataType dataType,
		const hmdbServices::ID id, const DataStatus & dataStatus)
	{
		manager->rawUpdateDataStatus(dataType, id, dataStatus);
	}

	virtual void removeDataStatus(const DataType dataType,
		const hmdbServices::ID id)
	{
		manager->rawRemoveDataStatus(dataType, id);
	}


	virtual void removeDataStatus(const DataType dataType)
	{
		manager->rawRemoveDataStatus(dataType);
	}

	virtual void tryUpdate(const ShallowCopyConstPtr shallowCopy)
	{
		manager->rawTryUpdate(shallowCopy);
	}

	virtual void rebuild(const IHMDBStorageConstPtr storage, const ShallowCopyConstPtr shallowCopy)
	{
		manager->rawRebuild(storage, shallowCopy);
	}

private:
	HMDBStatusManager * manager;
	threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> lock;
};

HMDBStatusManager::HMDBStatusManager()
{

}

HMDBStatusManager::~HMDBStatusManager()
{

}

const HMDBStatusManager::UpdateData HMDBStatusManager::fullUpdateData(const ShallowCopyConstPtr shallowCopy)
{
	UpdateData lud(updateData);

	auto it = updateData.find(FileType);
	if (it != updateData.end()){
		for (auto fit = it->second.begin(); fit != it->second.end(); ++fit){
			auto fIT = shallowCopy->motionShallowCopy.files.find(*fit);
			if (fIT != shallowCopy->motionShallowCopy.files.end()){
				if (fIT->second->isSessionFile() == true){
					lud[SessionType].insert(fIT->second->session->sessionID);
					updateData[SessionType].insert(fIT->second->session->sessionID);
				}
				else{
					lud[MotionType].insert(fIT->second->trial->trialID);
					updateData[MotionType].insert(fIT->second->trial->trialID);
				}
			}
		}
	}

	it = updateData.find(MotionType);
	if (it != updateData.end()){
		for (auto mit = it->second.begin(); mit != it->second.end(); ++mit){
			auto mIT = shallowCopy->motionShallowCopy.trials.find(*mit);
			if (mIT != shallowCopy->motionShallowCopy.trials.end()){
				lud[SessionType].insert(mIT->second->session->sessionID);
				updateData[SessionType].insert(mIT->second->session->sessionID);
			}
		}
	}

	it = updateData.find(SessionType);
	if (it != updateData.end()){
		for (auto sit = it->second.begin(); sit != it->second.end(); ++sit){
			auto sIT = shallowCopy->motionShallowCopy.sessions.find(*sit);
			if (sIT != shallowCopy->motionShallowCopy.sessions.end()){
				lud[SubjectType].insert(sIT->second->performerConf->performer->performerID);
				updateData[SubjectType].insert(sIT->second->performerConf->performer->performerID);
			}
		}
	}

	it = updateData.find(SubjectType);
	if (it != updateData.end()){
		for (auto sit = it->second.begin(); sit != it->second.end(); ++sit){
			auto sIT = shallowCopy->motionShallowCopy.performers.find(*sit);
			if (sIT != shallowCopy->motionShallowCopy.performers.end()
				&& sIT->second->patient != nullptr){

				lud[PatientType].insert(sIT->second->patient->patientID);
			}
		}
	}

	return lud;
}

void HMDBStatusManager::tryUpdate(const ShallowCopyConstPtr shallowCopy)
{
	ScopedLock lock(synch_);
	rawTryUpdate(shallowCopy);
}

void HMDBStatusManager::rawTryUpdate(const ShallowCopyConstPtr shallowCopy)
{
	if (shallowCopy != nullptr && updateData.empty() == false){

		auto fud = fullUpdateData(shallowCopy);

		auto it = fud.find(MotionType);

		if (it != fud.end()){
			for (auto mit = it->second.begin(); mit != it->second.end(); ++mit){
				auto ds = statuses[MotionType][*mit];
				auto motion = shallowCopy->motionShallowCopy.trials.find(*mit);

				if (motion == shallowCopy->motionShallowCopy.trials.end()){
					continue;
				}

				auto nds = DataStatus(ds.validity());				
				for (auto fit = motion->second->files.begin(); fit != motion->second->files.end() && nds != DataStatus::PartialStatus; ++fit){
					nds |= statuses[FileType][fit->first];
				}

				statuses[MotionType][*mit] = nds;
			}
		}

		it = fud.find(SessionType);

		if (it != fud.end()){
			for (auto sit = it->second.begin(); sit != it->second.end(); ++sit){
				auto ds = statuses[SessionType][*sit];
				auto session = shallowCopy->motionShallowCopy.sessions.find(*sit);

				if (session == shallowCopy->motionShallowCopy.sessions.end()){
					continue;
				}

				auto nds = DataStatus(ds.validity());				
				for (auto fit = session->second->files.begin(); fit != session->second->files.end() && nds != DataStatus::PartialStatus; ++fit){
					nds |= statuses[FileType][fit->first];
				}

				for (auto tit = session->second->trials.begin(); tit != session->second->trials.end() && nds != DataStatus::PartialStatus; ++tit){
					nds |= statuses[MotionType][tit->first];
				}

				statuses[SessionType][*sit] = nds;
			}
		}

		it = fud.find(SubjectType);

		if (it != fud.end()){
			for (auto sit = it->second.begin(); sit != it->second.end(); ++sit){
				auto ds = statuses[SubjectType][*sit];
				auto subject = shallowCopy->motionShallowCopy.performers.find(*sit);

				if (subject == shallowCopy->motionShallowCopy.performers.end()){
					continue;
				}

				auto nds = DataStatus(ds.validity());
				for (auto pit = subject->second->performerConfs.begin(); pit != subject->second->performerConfs.end() && nds != DataStatus::PartialStatus; ++pit){
					nds |= statuses[SessionType][pit->second->session->sessionID];
				}

				statuses[SubjectType][*sit] = nds;
			}
		}

		it = fud.find(PatientType);

		if (it != fud.end()){
			for (auto pit = it->second.begin(); pit != it->second.end(); ++pit){
				auto ds = statuses[PatientType][*pit];
				auto patient = shallowCopy->medicalShallowCopy.patients.find(*pit);
				auto nds = DataStatus(ds.validity());
				statuses[PatientType][*pit] = patient->second->performer != nullptr ? nds | statuses[SubjectType][patient->second->performer->performerID] : ds;
			}
		}

		UpdateData().swap(updateData);
	}
}

void HMDBStatusManager::rebuild(const IHMDBStorageConstPtr storage, const ShallowCopyConstPtr shallowCopy)
{
	ScopedLock lock(synch_);
	rawRebuild(storage, shallowCopy);
}

void HMDBStatusManager::rawRebuild(const IHMDBStorageConstPtr storage, const ShallowCopyConstPtr shallowCopy)
{
	Statuses locStatuses;

	{
		auto t = plugin::getStreamDataManagerReader()->transaction();
		auto keys = storage->transaction()->keys();

		for (auto it = shallowCopy->motionShallowCopy.files.begin();
			it != shallowCopy->motionShallowCopy.files.end(); ++it){

			auto st = keys.find(it->second->fileName) != keys.end() ? DataStatus::Local : DataStatus::Remote;
			auto usage = DataStatus::Unloaded;
			auto sName = core::IStreamManagerReader::streamName(storage->protocol(), it->second->fileName);
			if (t->isManaged(sName) == true){
				usage = DataStatus::PartiallyLoaded;
				if (t->isLoadedCompleately(sName) == true){
					usage = DataStatus::Loaded;
				}
			}

			auto validity = DataStatus::Valid;

			auto sIT = statuses[FileType].find(it->first);
			if (sIT != statuses[FileType].end()){
				validity = sIT->second.validity();
			}

			locStatuses[FileType][it->first] = DataStatus(st, usage, validity);
		}
	}

	for (auto it = shallowCopy->motionShallowCopy.trials.begin();
		it != shallowCopy->motionShallowCopy.trials.end(); ++it){

		auto validity = DataStatus::UnknownValidity;
		auto mIT = statuses[MotionType].find(it->first);
		if (mIT != statuses[MotionType].end()){
			validity = mIT->second.validity();
		}

		DataStatus mDs(validity);

		for (auto fit = it->second->files.begin();
			fit != it->second->files.end() && mDs != DataStatus::PartialStatus; ++fit){
			mDs |= locStatuses[FileType][fit->first];
		}

		locStatuses[MotionType][it->first] = mDs;
	}

	for (auto it = shallowCopy->motionShallowCopy.sessions.begin();
		it != shallowCopy->motionShallowCopy.sessions.end(); ++it){
		
		auto validity = DataStatus::UnknownValidity;
		auto mIT = statuses[SessionType].find(it->first);
		if (mIT != statuses[SessionType].end()){
			validity = mIT->second.validity();
		}

		DataStatus mDs(validity);

		for (auto fit = it->second->files.begin();
			fit != it->second->files.end() && mDs != DataStatus::PartialStatus; ++fit){
			mDs |= locStatuses[FileType][fit->first];
		}

		for (auto fit = it->second->trials.begin();
			fit != it->second->trials.end() && mDs != DataStatus::PartialStatus; ++fit){
			mDs |= locStatuses[MotionType][fit->first];
		}

		locStatuses[SessionType][it->first] = mDs;
	}

	for (auto it = shallowCopy->motionShallowCopy.performers.begin();
		it != shallowCopy->motionShallowCopy.performers.end(); ++it){

		auto validity = DataStatus::UnknownValidity;
		auto mIT = statuses[SubjectType].find(it->first);
		if (mIT != statuses[SubjectType].end()){
			validity = mIT->second.validity();
		}

		DataStatus mDs(validity);

		for (auto fit = it->second->performerConfs.begin();
			fit != it->second->performerConfs.end() && mDs != DataStatus::PartialStatus; ++fit){
			mDs |= locStatuses[SessionType][fit->second->session->sessionID];
		}

		locStatuses[SubjectType][it->first] = mDs;
	}

	for (auto it = shallowCopy->medicalShallowCopy.patients.begin();
		it != shallowCopy->medicalShallowCopy.patients.end(); ++it){

		auto validity = DataStatus::UnknownValidity;
		auto mIT = statuses[PatientType].find(it->first);
		if (mIT != statuses[PatientType].end()){
			validity = mIT->second.validity();
		}

		DataStatus mDs(validity);

		if (it->second->performer != nullptr){
			auto sit = locStatuses[SubjectType].find(it->second->performer->performerID);
			if (sit != locStatuses[SubjectType].end()){
				mDs |= sit->second;
			}
		}

		locStatuses[PatientType][it->first] = mDs;
	}


	//teraz brakujace wpisy
	for (auto it = statuses.begin(); it != statuses.end(); ++it){

		if (it->second.empty() == false){
			locStatuses[it->first];
			auto dIT = locStatuses.find(it->first);

			for (auto sit = it->second.begin(); sit != it->second.end(); ++sit){
				if (dIT->second.find(sit->first) == dIT->second.end()){
					dIT->second.insert(*sit);
				}
			}
		}
	}

	//teraz podmieniam
	locStatuses.swap(statuses);
	UpdateData().swap(updateData);
}

const HMDBStatusManager::TransactionPtr HMDBStatusManager::transaction()
{
	return HMDBStatusManager::TransactionPtr(new HMDBStatusManagerTransaction(this));
}

const HMDBStatusManager::TransactionConstPtr HMDBStatusManager::transaction() const
{
	return HMDBStatusManager::TransactionConstPtr(new HMDBStatusManagerTransaction(const_cast<HMDBStatusManager*>(this)));
}

const DataStatus HMDBStatusManager::dataStatus(const DataType dataType,
	const hmdbServices::ID id) const
{
	ScopedLock lock(synch_);
	return rawDataStatus(dataType, id);
}

void HMDBStatusManager::setDataStatus(const DataType dataType,
	const hmdbServices::ID id, const DataStatus & dataStatus)
{
	ScopedLock lock(synch_);
	rawSetDataStatus(dataType, id, dataStatus);
}

void HMDBStatusManager::updateDataStatus(const DataType dataType,
	const hmdbServices::ID id, const DataStatus & dataStatus)
{
	ScopedLock lock(synch_);
	rawUpdateDataStatus(dataType, id, dataStatus);
}

void HMDBStatusManager::removeDataStatus(const DataType dataType,
	const hmdbServices::ID id)
{
	ScopedLock lock(synch_);
	rawRemoveDataStatus(dataType, id);
}

void HMDBStatusManager::removeDataStatus(const DataType dataType)
{
	ScopedLock lock(synch_);
	rawRemoveDataStatus(dataType);
}
	
const DataStatus HMDBStatusManager::rawDataStatus(const DataType dataType,
	const hmdbServices::ID id) const
{
	DataStatus ret;
	auto it = statuses.find(dataType);
	if (it != statuses.end()){
		auto sit = it->second.find(id);
		if (sit != it->second.end()){
			ret = sit->second;
		}
	}

	return ret;
}

void HMDBStatusManager::rawSetDataStatus(const DataType dataType,
	const hmdbServices::ID id, const DataStatus & dataStatus)
{
	statuses[dataType][id] = dataStatus;
	updateData[dataType].insert(id);
}

void HMDBStatusManager::rawUpdateDataStatus(const DataType dataType,
	const hmdbServices::ID id, const DataStatus & dataStatus)
{
	auto it = statuses.find(dataType);
	if (it != statuses.end()){
		auto IT = it->second.find(id);
		if (IT != it->second.end()){

			if (dataStatus.storage() != DataStatus::UnknownStorage){
				IT->second.setStorage(dataStatus.storage());
			}

			if (dataStatus.usage() != DataStatus::UnknownUsage){
				IT->second.setUsage(dataStatus.usage());
			}

			if (dataStatus.validity() != DataStatus::UnknownValidity){
				IT->second.setValidity(dataStatus.validity());
			}

			updateData[dataType].insert(id);
			return;
		}
	}

	rawSetDataStatus(dataType, id, dataStatus);
}

void HMDBStatusManager::rawRemoveDataStatus(const DataType dataType,
	const hmdbServices::ID id)
{
	auto it = statuses.find(dataType);
	if (it != statuses.end()){
		it->second.erase(id);
	}
}

void HMDBStatusManager::rawRemoveDataStatus(const DataType dataType)
{
	statuses.erase(dataType);
}