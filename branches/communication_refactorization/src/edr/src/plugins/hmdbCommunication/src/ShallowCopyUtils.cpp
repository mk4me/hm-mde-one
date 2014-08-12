#include "CommunicationPCH.h"
#include "ShallowCopyUtils.h"

#include <hmdbserviceslib/ShallowCopyParser.h>
#include <hmdbserviceslib/MetadataParser.h>
#include <plugins/hmdbCommunication/IHMDBStorage.h>
#include <boost/regex.hpp>

using namespace hmdbCommunication;

void ShallowCopyUtils::createShallowCopy(hmdbCommunication::ShallowCopy & shallowCopy, std::istream * motionShallowCopy,
	std::istream * motionMetadata, std::istream * medicalShallowCopy,
	std::istream * medicalMetadata)
{
	using namespace hmdbServices;

	hmdbCommunication::ShallowCopy tmpShallowCopy;

	//parsujemy
	MotionShallowCopyParser::parseFile(motionShallowCopy, tmpShallowCopy.motionShallowCopy);
	MedicalShallowCopyParser::parseFile(medicalShallowCopy, tmpShallowCopy.medicalShallowCopy);
	MotionMetadataParser::parseFile(motionMetadata, tmpShallowCopy.motionMetaData);
	MedicalMetadataParser::parseFile(medicalMetadata, tmpShallowCopy.medicalMetaData);

	//mamy teoretycznie wszystko - teraz próbuje połączyć obie bazy
	auto subjectsITEnd = tmpShallowCopy.motionShallowCopy.performers.end();
	auto patientsITEnd = tmpShallowCopy.medicalShallowCopy.patients.end();
	for (auto patientIT = tmpShallowCopy.medicalShallowCopy.patients.begin(); patientIT != patientsITEnd; ++patientIT){
		auto subjectIT = tmpShallowCopy.motionShallowCopy.performers.find(patientIT->second->motionPerformerID);
		if (subjectIT != subjectsITEnd){
			patientIT->second->performer = subjectIT->second;
			subjectIT->second->patient = patientIT->second;
		}
	}

	// jeśli wszystko się powiodło podmieniamy wartości
	std::swap(shallowCopy, tmpShallowCopy);
}

const ShallowCopyPtr ShallowCopyUtils::extractShallowCopy(const std::string & userHash,
	const IHMDBStorageOperations * storage)
{
	ShallowCopyPtr ret(new ShallowCopy);

	utils::scoped_ptr<std::istream> stream(shallowCopyStream(userHash, IHMDBRemoteContext::Medical, Shallow, storage));
	if (stream != nullptr){
		hmdbServices::MedicalShallowCopyParser::parseFile(stream.get(), ret->medicalShallowCopy);
	}

	stream.reset(shallowCopyStream(userHash, IHMDBRemoteContext::Medical, Meta, storage));
	if (stream != nullptr){
		hmdbServices::MedicalMetadataParser::parseFile(stream.get(), ret->medicalMetaData);
	}

	stream.reset(shallowCopyStream(userHash, IHMDBRemoteContext::Motion, Shallow, storage));
	if (stream != nullptr){
		hmdbServices::MotionShallowCopyParser::parseFile(stream.get(), ret->motionShallowCopy);
	}

	stream.reset(shallowCopyStream(userHash, IHMDBRemoteContext::Motion, Meta, storage));
	if (stream != nullptr){
		hmdbServices::MotionMetadataParser::parseFile(stream.get(), ret->motionMetaData);
	}

	return ret;
}

const bool ShallowCopyUtils::checkShallowCopyIntegrity(const ShallowCopy & shallowCopy)
{
	//TODO
	//sprawdzać integralność danych w płytkiej kopii bazy danych
	return true;
}

const bool ShallowCopyUtils::shallowCopyRequiresRefresh(const ShallowCopy & shallowCopy,
	const hmdbServices::DateTime & currentVersion)
{
	return ( (shallowCopy.motionShallowCopy.timestamp < currentVersion) ? true : false);
}

const bool ShallowCopyUtils::shallowCopyInStorage(const std::string & userHash,
	const IHMDBRemoteContext::DataReference dataReference,
	const ShallowCopyType shallowType,
	const IHMDBStorageOperations * storage)
{
	return storage->exists(shallowCopyName(userHash, dataReference, shallowType));
}

void ShallowCopyUtils::removeAllShallowCopies(IHMDBStorageOperations * storage)
{
	auto scfs = allShallowCopiesNames(storage);

	for (auto it = scfs.begin(); it != scfs.end(); ++it){
		storage->remove(*it);
	}
}

void ShallowCopyUtils::storeShallowCopy(const std::string & userHash,
	const IHMDBRemoteContext::DataReference dataReference,
	const ShallowCopyType shallowType, std::istream * stream, IHMDBStorageOperations * storage)
{
	const auto scname = shallowCopyName(userHash, dataReference, shallowType);
	if (scname.empty() == false){
		storage->set(scname, stream);
	}
}

std::istream * ShallowCopyUtils::shallowCopyStream(const std::string & userHash,
	const IHMDBRemoteContext::DataReference dataReference,
	const ShallowCopyType shallowType, const IHMDBStorageOperations * storage)
{
	return storage->get(shallowCopyName(userHash, dataReference, shallowType));
}

const std::string ShallowCopyUtils::shallowCopyName(const std::string & userHash,
	const IHMDBRemoteContext::DataReference dataReference,
	const ShallowCopyType shallowType)
{
	std::string ret("m");

	if (dataReference == IHMDBRemoteContext::Medical){
		ret += "ed";
	}

	ret += "_" + userHash + "_";

	switch (shallowType)
	{
	case Shallow:
		ret += "s";
		break;

	case Meta:
		ret += "m";
		break;

	case IncrementalShallow:
		ret += "incs";
		break;

	case IncrementalMeta:
		ret += "incm";
		break;

	default:
		std::string().swap(ret);
		break;
	}

	if (ret.empty() == false){
		ret += ".xml.shallow";
	}

	return ret;
}

const std::list<std::string> ShallowCopyUtils::allShallowCopiesNames(IHMDBStorageOperations * storage)
{
	static const boost::regex r("^(med|m)_[[::alpha::]*?_[inc](s|m)\.xml\.shallow$");

	std::list<std::string> ret;

	auto sfiles = storage->keys();

	for (auto it = sfiles.begin(); it != sfiles.end(); ++it){
		boost::smatch what;
		if (boost::regex_match(*it, what, r) == true){

			ret.push_back(*it);
		}
	}

	return ret;
}

const std::list<std::string> ShallowCopyUtils::filterShallowCopiesNames(const std::list<std::string> & src,
	const IHMDBRemoteContext::DataReference dataReference,
	const ShallowCopyType shallowType)
{
	std::string start;

	switch (dataReference){
	case IHMDBRemoteContext::Motion:
		start = "m";
		break;
	case IHMDBRemoteContext::Medical:
		start = "med";
		break;
	}

	std::string end;

	switch (shallowType)
	{
	case ShallowCopyUtils::Shallow:
		end = "s";
		break;
	case ShallowCopyUtils::Meta:
		end = "m";
		break;
	case ShallowCopyUtils::IncrementalShallow:
		end = "incs";
		break;
	case ShallowCopyUtils::IncrementalMeta:
		end = "incm";
		break;
	}

	boost::regex r("^" + start + "_[[::alpha::]*?_" + end + "\.xml\.shallow$");

	std::list<std::string> ret;

	for (auto it = src.begin(); it != src.end(); ++it){
		boost::smatch what;
		if (boost::regex_match(*it, what, r) == true){

			ret.push_back(*it);
		}
	}

	return ret;
}

const std::list<std::string> ShallowCopyUtils::removeUserShallowCopiesNames(const std::list<std::string> & src,
	const std::string & userHash)
{
	std::list<std::string> ret;

	for (auto it = src.begin(); it != src.end(); ++it){

		auto s = (*it).find("_") + 1;
		auto e = (*it).rfind("_");

		if ((*it).substr(s, e - s) != userHash){
			ret.push_back(*it);
		}
	}

	return ret;
}

const std::string ShallowCopyUtils::userHash(const std::string & user, const std::string & password)
{
	std::string hashTmp(user + ":" + password);
	std::string hash;
	hash.resize(hashTmp.size());

	for (int i = 0; i < hashTmp.size(); ++i){
		hash[i] = hashTmp[(i + 5) % hashTmp.size()];
	}

	return hash;
}

const ShallowCopyUtils::StorageUserFiles ShallowCopyUtils::groupedShallowCopiesNames(IHMDBStorageOperations * storage)
{
	StorageUserFiles ret;

	const auto sfiles = allShallowCopiesNames(storage);

	for (auto it = sfiles.begin(); it != sfiles.end(); ++it){
		IHMDBRemoteContext::DataReference dr = IHMDBRemoteContext::Motion;
		int fileID = 0;

		auto spos = (*it).find("med_");
		if (spos == 0){
			dr = IHMDBRemoteContext::Medical;
			spos = 4;
		}
		else{
			spos = 2;
		}

		auto epos = (*it).find_last_of(".xml.shallow") - 1;

		if (epos == 'm'){
			fileID = MetadataFileID;
		}
		else{
			fileID = ShallowCopyFileID;
		}

		const std::string userHash((*it).substr(spos, epos - spos - 1));

		ret[userHash][dr][fileID] = *it;
	}

	return ret;
}

const ShallowCopyUtils::StorageFileNames ShallowCopyUtils::files(const ShallowCopy & shallowCopy)
{
	StorageFileNames ret;

	for (auto it = shallowCopy.motionShallowCopy.files.begin();
		it != shallowCopy.motionShallowCopy.files.end(); ++it){
		ret[it->first] = it->second->fileName;
	}

	return ret;
}

void extractFiles(ShallowCopyUtils::StorageFileNames & storageFiles,
	const hmdbServices::MotionShallowCopy::Files & files)
{
	for (auto fit = files.begin(); fit != files.end(); ++fit){
		storageFiles[fit->first] = fit->second->fileName;
	}
}

void extractFiles(ShallowCopyUtils::StorageFileNames & storageFiles,
	const hmdbServices::MotionShallowCopy::Trial * motion)
{
	extractFiles(storageFiles, motion->files);
}

void extractFiles(ShallowCopyUtils::StorageFileNames & storageFiles,
	const hmdbServices::MotionShallowCopy::Session * session, const bool recursive)
{
	extractFiles(storageFiles, session->files);

	if (recursive == true){
		for (auto it = session->trials.begin(); it != session->trials.end(); ++it){
			extractFiles(storageFiles, it->second);
		}
	}
}

void extractFiles(ShallowCopyUtils::StorageFileNames & storageFiles,
	const hmdbServices::MotionShallowCopy::Performer * subject, const bool recursive)
{
	if (recursive == true){
		for (auto it = subject->performerConfs.begin(); it != subject->performerConfs.end(); ++it){
			extractFiles(storageFiles, it->second->session, true);
		}
	}
}

void extractFiles(ShallowCopyUtils::StorageFileNames & storageFiles,
	const hmdbServices::MedicalShallowCopy::Patient * patient, const bool recursive)
{
	if (recursive == true && patient->performer != nullptr){
		extractFiles(storageFiles, patient->performer, true);
	}
}

const ShallowCopyUtils::StorageFileNames ShallowCopyUtils::files(const DataType dataType,
	const ShallowCopy & shallowCopy, const bool recursive)
{
	StorageFileNames ret;

	switch (dataType){
	case FileType:
		ret = files(shallowCopy);
		break;

	case MotionType:

		for (auto it = shallowCopy.motionShallowCopy.trials.begin();
			it != shallowCopy.motionShallowCopy.trials.end(); ++it){
			extractFiles(ret, it->second);
		}

		break;

	case SessionType:

		for (auto it = shallowCopy.motionShallowCopy.sessions.begin();
			it != shallowCopy.motionShallowCopy.sessions.end(); ++it){
			extractFiles(ret, it->second, recursive);
		}

		break;

	case SubjectType:

		for (auto it = shallowCopy.motionShallowCopy.performers.begin();
			it != shallowCopy.motionShallowCopy.performers.end(); ++it){
			extractFiles(ret, it->second, recursive);
		}
		break;

	case PatientType:

		for (auto it = shallowCopy.medicalShallowCopy.patients.begin();
			it != shallowCopy.medicalShallowCopy.patients.end(); ++it){
			extractFiles(ret, it->second, recursive);
		}
		break;
	}

	return ret;
}

const ShallowCopyUtils::StorageFileNames ShallowCopyUtils::files(const DataType dataType,
	const hmdbServices::ID id, const ShallowCopy & shallowCopy, const bool recursive)
{
	StorageFileNames ret;

	switch (dataType)
	{
	case FileType:
	{
		auto it = shallowCopy.motionShallowCopy.files.find(id);
		if (it != shallowCopy.motionShallowCopy.files.end()){
			ret[id] = it->second->fileName;
		}
	}
		break;

	case MotionType:
	{
		auto it = shallowCopy.motionShallowCopy.trials.find(id);
		if (it != shallowCopy.motionShallowCopy.trials.end()){
			extractFiles(ret, it->second);
		}
	}
		break;

	case SessionType:
	{
		auto it = shallowCopy.motionShallowCopy.sessions.find(id);
		if (it != shallowCopy.motionShallowCopy.sessions.end()){
			extractFiles(ret, it->second, recursive);
		}
	}
		break;

	case SubjectType:
	{
		auto it = shallowCopy.motionShallowCopy.performers.find(id);
		if (it != shallowCopy.motionShallowCopy.performers.end()){
			extractFiles(ret, it->second, recursive);
		}
	}
		break;

	case PatientType:
	{
		auto it = shallowCopy.medicalShallowCopy.patients.find(id);
		if (it != shallowCopy.medicalShallowCopy.patients.end()){
			extractFiles(ret, it->second, recursive);
		}
	}
		break;
	}

	return ret;
}