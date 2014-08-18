#include "CommunicationPCH.h"
#include "HMDBShallowCopyContext.h"
#include "HMDBSourceContextOperations.h"
#include "HMDBStatusManager.h"
#include <plugins/hmdbCommunication/IHMDBLocalContext.h>
#include <plugins/hmdbCommunication/IPatient.h>
#include "Patient.h"

#include <hmdbserviceslib/DateTimeUtils.h>

#include <plugins/subject/ISubjectService.h>
#include <plugins/subject/ISubject.h>
#include <plugins/subject/ISession.h>
#include <plugins/subject/IMotion.h>

#include <plugins/kinematic/Wrappers.h>
#include <plugins/video/Wrappers.h>
#include <plugins/c3d/C3DCollections.h>

#include <corelib/PluginCommon.h>
#include <corelib/IServiceManager.h>

using namespace hmdbCommunication;

HMDBShallowCopyDataContext::HMDBShallowCopyDataContext() : statusManager_(new HMDBStatusManager)
{

}

HMDBShallowCopyDataContext::~HMDBShallowCopyDataContext()
{

}

const ShallowCopyConstPtr HMDBShallowCopyDataContext::shallowCopy() const
{
	return shallowCopy_;
}

const IHMDBStatusManagerConstPtr HMDBShallowCopyDataContext::dataStatusManager() const
{
	return statusManager_;
}

const IHMDBStatusManagerPtr HMDBShallowCopyDataContext::dataStatusManager()
{
	return statusManager_;
}

void HMDBShallowCopyDataContext::setShallowCopy(const ShallowCopyConstPtr shallowCopy)
{
	shallowCopy_ = shallowCopy;
	if (shallowCopy_ != nullptr){
		//statusManager_->rebuild(shallowCopy_);
	}
}

HMDBShallowCopyLocalContext::HMDBShallowCopyLocalContext(IHMDBShallowCopyDataContextPtr shallowCopyContext,
	IHMDBLocalContextPtr localContext) : shallowCopyContext_(shallowCopyContext),
	localContext_(localContext)
{

}

HMDBShallowCopyLocalContext::~HMDBShallowCopyLocalContext()
{
	unloadAll();
}

void filesIDs(const hmdbServices::MotionShallowCopy::Files & files,
	std::set<hmdbServices::ID> & fileIDs)
{
	for (auto it = files.begin(); it != files.end(); ++it){
		fileIDs.insert(it->second->fileID);
	}
}

void motionsToFiles(const hmdbServices::MotionShallowCopy::Trials & trials,
	std::set<hmdbServices::ID> & files)
{
	for (auto it = trials.begin(); it != trials.end(); ++it){
		filesIDs(it->second->files, files);
	}
}

void sessionToFiles(const hmdbServices::MotionShallowCopy::Session * session,
	std::set<hmdbServices::ID> & files)
{
	filesIDs(session->files, files);
	motionsToFiles(session->trials, files);
}

void subjectToFiles(const hmdbServices::MotionShallowCopy::Performer * subject,
	std::set<hmdbServices::ID> & files)
{
	for (auto it = subject->performerConfs.begin(); it != subject->performerConfs.end(); ++it)
	{
		sessionToFiles(it->second->session, files);
	}
}

const std::set<hmdbServices::ID> filesToProcess(const DataType type,
	const hmdbServices::ID id, const ShallowCopyConstPtr shallowCopy)
{
	std::set<hmdbServices::ID> ret;

	if (id < 0){
		filesIDs(shallowCopy->motionShallowCopy.files, ret);
	}
	else{

		switch (type)
		{
		case FileType:

		{
			auto it = shallowCopy->motionShallowCopy.files.find(id);
			if (it != shallowCopy->motionShallowCopy.files.end()){
				ret.insert(id);
			}
		}

			break;

		case MotionType:

		{
			auto it = shallowCopy->motionShallowCopy.trials.find(id);
			if (it != shallowCopy->motionShallowCopy.trials.end()){
				filesIDs(it->second->files, ret);
			}
		}

			break;

		case SessionType:

		{
			auto it = shallowCopy->motionShallowCopy.sessions.find(id);
			if (it != shallowCopy->motionShallowCopy.sessions.end()){
				sessionToFiles(it->second, ret);
			}
		}

			break;

		case SubjectType:

		{
			auto it = shallowCopy->motionShallowCopy.performers.find(id);
			if (it != shallowCopy->motionShallowCopy.performers.end()){
				subjectToFiles(it->second, ret);
			}
		}

			break;

		case PatientType:

		{
			auto it = shallowCopy->medicalShallowCopy.patients.find(id);
			if (it != shallowCopy->medicalShallowCopy.patients.end()){
				if (it->second->performer != nullptr){
					subjectToFiles(it->second->performer, ret);
				}
			}
		}

			break;

		}
	}

	return ret;
}

void childItems(const hmdbServices::MotionShallowCopy::Files & files,
	std::set<hmdbServices::ID> & fileIDs)
{
	for (auto IT = files.begin(); IT != files.end(); ++IT){
		fileIDs.insert(IT->second->fileID);
	}
}

void childItems(const hmdbServices::MotionShallowCopy::Trials & motions,
	std::set<hmdbServices::ID> & fileIDs)
{
	for (auto IT = motions.begin(); IT != motions.end(); ++IT){
		childItems(IT->second->files, fileIDs);
	}
}

void childItems(const hmdbServices::MotionShallowCopy::Session * session,
	std::set<hmdbServices::ID> & motionIDs,
	std::set<hmdbServices::ID> & fileIDs)
{
	childItems(session->files, fileIDs);	

	for (auto IT = session->trials.begin(); IT != session->trials.end(); ++IT){
		motionIDs.insert(IT->second->trialID);
		childItems(IT->second->files, fileIDs);
	}
}

void childItems(const hmdbServices::MotionShallowCopy::Sessions sessions,
	std::set<hmdbServices::ID> & motionIDs,
	std::set<hmdbServices::ID> & fileIDs)
{
	for (auto IT = sessions.begin(); IT != sessions.end(); ++IT){
		childItems(IT->second, motionIDs, fileIDs);
	}
}

void childItems(const hmdbServices::MotionShallowCopy::Performer * subject,
	std::set<hmdbServices::ID> & sessionIDs,
	std::set<hmdbServices::ID> & motionIDs,
	std::set<hmdbServices::ID> & fileIDs)
{
	for (auto IT = subject->performerConfs.begin(); IT != subject->performerConfs.end(); ++IT){
		childItems(IT->second->session, motionIDs, fileIDs);
		sessionIDs.insert(IT->second->session->sessionID);
	}
}

void childItems(const hmdbServices::MotionShallowCopy::Performers subjects,
	std::set<hmdbServices::ID> & sessionIDs,
	std::set<hmdbServices::ID> & motionIDs,
	std::set<hmdbServices::ID> & fileIDs)
{
	for (auto IT = subjects.begin(); IT != subjects.end(); ++IT){
		childItems(IT->second, sessionIDs, motionIDs, fileIDs);
	}
}

void childItems(const hmdbServices::MedicalShallowCopy::Patient * patient,
	std::set<hmdbServices::ID> & subjectsIDs,
	std::set<hmdbServices::ID> & sessionIDs,
	std::set<hmdbServices::ID> & motionIDs,
	std::set<hmdbServices::ID> & fileIDs)
{
	if (patient->performer != nullptr){
		subjectsIDs.insert(patient->performer->performerID);
		childItems(patient->performer, sessionIDs, motionIDs, fileIDs);
	}	
}

void childItems(const hmdbServices::MedicalShallowCopy::Patients & patients,
	std::set<hmdbServices::ID> & subjectsIDs,
	std::set<hmdbServices::ID> & sessionIDs,
	std::set<hmdbServices::ID> & motionIDs,
	std::set<hmdbServices::ID> & fileIDs)
{
	for (auto IT = patients.begin(); IT != patients.end(); ++IT){
		childItems(IT->second, subjectsIDs, sessionIDs, motionIDs, fileIDs);
	}
}

const std::map<DataType, std::set<hmdbServices::ID>> childItems(const DataType type,
	const hmdbServices::ID id, const ShallowCopyConstPtr shallowCopy)
{
	std::map<DataType, std::set<hmdbServices::ID>> ret;	

	switch (type)
	{
	case MotionType:

	{
		std::set<hmdbServices::ID> fileIDs;

		if (id < 0){
			childItems(shallowCopy->motionShallowCopy.trials, fileIDs);
		}
		else{
			auto it = shallowCopy->motionShallowCopy.trials.find(id);
			if (it != shallowCopy->motionShallowCopy.trials.end()){
				childItems(it->second->files, fileIDs);
			}
		}

		if (fileIDs.empty() == false){
			ret[FileType].insert(fileIDs.begin(), fileIDs.end());
		}
	}

		break;

	case SessionType:

	{
		std::set<hmdbServices::ID> fileIDs;
		std::set<hmdbServices::ID> motionIDs;

		if (id < 0){
			childItems(shallowCopy->motionShallowCopy.sessions, motionIDs, fileIDs);
		}
		else{
			auto it = shallowCopy->motionShallowCopy.sessions.find(id);
			if (it != shallowCopy->motionShallowCopy.sessions.end()){
				childItems(it->second, motionIDs, fileIDs);
			}
		}

		if (motionIDs.empty() == false){
			ret[MotionType].insert(motionIDs.begin(), motionIDs.end());
		}

		if (fileIDs.empty() == false){
			ret[FileType].insert(fileIDs.begin(), fileIDs.end());
		}
	}

		break;

	case SubjectType:

	{
		std::set<hmdbServices::ID> fileIDs;
		std::set<hmdbServices::ID> motionIDs;
		std::set<hmdbServices::ID> sessionIDs;

		if (id < 0){
			childItems(shallowCopy->motionShallowCopy.performers, sessionIDs, motionIDs, fileIDs);
		}
		else{
			auto it = shallowCopy->motionShallowCopy.performers.find(id);
			if (it != shallowCopy->motionShallowCopy.performers.end()){
				childItems(it->second, sessionIDs, motionIDs, fileIDs);
			}
		}

		if (sessionIDs.empty() == false){
			ret[SessionType].insert(sessionIDs.begin(), sessionIDs.end());
		}

		if (motionIDs.empty() == false){
			ret[MotionType].insert(motionIDs.begin(), motionIDs.end());
		}

		if (fileIDs.empty() == false){
			ret[FileType].insert(fileIDs.begin(), fileIDs.end());
		}
	}

		break;

	case PatientType:

	{
		std::set<hmdbServices::ID> fileIDs;
		std::set<hmdbServices::ID> motionIDs;
		std::set<hmdbServices::ID> sessionIDs;
		std::set<hmdbServices::ID> subjectIDs;

		if (id < 0){
			childItems(shallowCopy->medicalShallowCopy.patients, subjectIDs, sessionIDs, motionIDs, fileIDs);
		}
		else{
			auto it = shallowCopy->medicalShallowCopy.patients.find(id);
			if (it != shallowCopy->medicalShallowCopy.patients.end()){
				childItems(it->second, subjectIDs, sessionIDs, motionIDs, fileIDs);
			}
		}

		if (subjectIDs.empty() == false){
			ret[SubjectType].insert(subjectIDs.begin(), subjectIDs.end());
		}

		if (sessionIDs.empty() == false){
			ret[SessionType].insert(sessionIDs.begin(), sessionIDs.end());
		}

		if (motionIDs.empty() == false){
			ret[MotionType].insert(motionIDs.begin(), motionIDs.end());
		}

		if (fileIDs.empty() == false){
			ret[FileType].insert(fileIDs.begin(), fileIDs.end());
		}
	}

		break;

	}

	return ret;
}

void filterAndInsertData(core::ConstVariantsList & inputData, core::ConstVariantsList & outputData,
	const IHMDBLocalContextConstPtr localContext)
{
	auto it = inputData.begin();
	while (it != inputData.end()){
		if (localContext->isMyData(*it) == false){
			it = inputData.erase(it);
		}
		else{
			outputData.push_back(*it);
			++it;
		}
	}
}

const core::ConstVariantsList HMDBShallowCopyLocalContext::data(const DataType type,
	const hmdbServices::ID id, const bool recursive) const
{
	core::ConstVariantsList ret;

	std::map<DataType, std::set<hmdbServices::ID>> items;

	if (recursive == true){
		items = childItems(type, id, shallowCopyContext_->shallowCopy());
	}

	items[type].insert(id);

	for (auto it = items.begin(); it != items.end(); ++it){
		auto dIT = data_.find(type);
		// czy mam tak¹ grupê obiektów
		if (dIT != data_.end()){
			//czy chcê dane wszystkich obiektów tego typu?
			if (it->second.find(-1) != it->second.end()){
				// dla wszystkich indeksów
				auto IT = dIT->second.begin();
				while(IT != dIT->second.end()){
					//filtrowanie danych które zosta³y wy³adowane
					filterAndInsertData(IT->second, ret, localContext_);
					if (IT->second.empty() == true){
						IT = dIT->second.erase(IT);
					}
					else{
						++IT;
					}
				}
				//czy grupa pusta?
				if (dIT->second.empty() == true){
					data_.erase(dIT);
				}
			}
			else{
				auto IT = it->second.begin();
				
				while(IT != it->second.end()){
					auto tIT = dIT->second.find(*IT);
					if (tIT != dIT->second.end()){
						filterAndInsertData(tIT->second, ret, localContext_);

						if (tIT->second.empty() == true){
							dIT->second.erase(tIT);
							if (dIT->second.empty() == true){
								data_.erase(dIT);
							}							
						}
					}
					
					++IT;					
				}
			}
		}
	}

	return ret;
}

const std::set<hmdbServices::ID> filterFiles(const std::set<hmdbServices::ID> & inFiles,
	const IHMDBStatusManagerConstPtr statusManager, const DataStatus & status)
{
	std::set<hmdbServices::ID> ret;
	auto transaction = statusManager->transaction();

	for (auto it = inFiles.begin(); it != inFiles.end(); ++it){
		auto fs = transaction->dataStatus(FileType, *it);
		if (DataStatus::filter(fs, status) == true){
			ret.insert(*it);
		}
	}

	return ret;
}

class JointsInitializer : public core::IVariantInitializer
{
public:

	JointsInitializer(core::VariantConstPtr dataWrapper,
		core::VariantConstPtr modelWrapper)
		: dataWrapper(dataWrapper), modelWrapper(modelWrapper)
	{
	}

	virtual ~JointsInitializer()
	{
	}

	virtual void initialize(core::Variant * object)
	{
		kinematic::SkeletalDataConstPtr data;
		kinematic::SkeletalModelConstPtr model;
		if (dataWrapper->tryGet(data) == true && modelWrapper->tryGet(model) == true && data != nullptr && model != nullptr){
			kinematic::JointAnglesCollectionPtr joints(new kinematic::JointAnglesCollection());
			joints->setSkeletal(model, data);
			object->set(joints);
		}
	}

	virtual IVariantInitializer * clone() const
	{
		return new JointsInitializer(dataWrapper, modelWrapper);
	}

private:
	core::VariantConstPtr dataWrapper;
	core::VariantConstPtr modelWrapper;
};

void HMDBShallowCopyLocalContext::addPatientObject(const hmdbServices::MedicalShallowCopy::Patient * patient, PluginSubject::SubjectID subjectID,
	core::IMemoryDataManager::IMemoryDataTransaction * transaction)
{
	//generujê listê schorzeñ
	std::vector<hmdbCommunication::Disorder> disorders;
	for (auto it = patient->disorders.begin(); it != patient->disorders.end(); ++it)	{
		hmdbCommunication::Disorder dis;
		dis.focus = it->second.focus;
		dis.diagnosisDate = hmdbServices::toString(it->second.diagnosisDate);
		dis.comments = it->second.comments;
		dis.name = it->second.disorder->name;

		disorders.push_back(dis);
	}

	PatientPtr pPtr(new Patient(subjectID, patient->name, patient->surname, patient->birthDate.toString(),
		Patient::decodeGender(patient->gender), utils::shared_ptr<const QPixmap>(), disorders));

	//dodajê do DM
	auto pOW = core::Variant::create<hmdbCommunication::IPatient>();
	pOW->set(pPtr);

	transaction->addData(pOW);

	//zapamiêtuje
	data_[PatientType][patient->patientID].push_back(pOW);
}

const float getAntropometricValue(const std::string & attribute, const hmdbServices::MotionShallowCopy::Attrs & attrs, float defValue = 0.0)
{
	auto it = attrs.find(attribute);
	if (it != attrs.end()){
		return boost::lexical_cast<float>(it->second);
	}

	return defValue;
}

utils::shared_ptr<hmdbCommunication::AntropometricData> createAntropometricData(const hmdbServices::MotionShallowCopy::Attrs & attrs)
{
	auto antro = utils::shared_ptr<hmdbCommunication::AntropometricData>(new hmdbCommunication::AntropometricData());
	antro->bodyMass = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("BodyMass", attrs), "kg");
	antro->height = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("Height", attrs), "mm");
	antro->interAsisDistance = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("InterAsisDistance", attrs), "mm");

	antro->leftLegLength = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("LeftLegLength", attrs), "mm");
	antro->rightLegLenght = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("RightLegLenght", attrs), "mm");

	antro->leftKneeWidth = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("LeftKneeWidth", attrs), "mm");
	antro->rightKneeWidth = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("RightKneeWidth", attrs), "mm");

	antro->leftAnkleWidth = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("LeftAnkleWidth", attrs), "mm");
	antro->rightAnkleWidth = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("RightAnkleWidth", attrs), "mm");

	antro->leftCircuitThigh = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("LeftCircuitThigh", attrs), "mm");
	antro->rightCircuitThight = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("RightCircuitThight", attrs), "mm");

	antro->leftCircuitShank = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("LeftCircuitShank", attrs), "mm");
	antro->rightCircuitShank = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("RightCircuitShank", attrs), "mm");

	antro->leftShoulderOffset = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("LeftShoulderOffset", attrs), "mm");
	antro->rightShoulderOffset = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("RightShoulderOffset", attrs), "mm");

	antro->leftElbowWidth = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("LeftElbowWidth", attrs), "mm");
	antro->rightElbowWidth = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("RightElbowWidth", attrs), "mm");

	antro->leftWristWidth = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("LeftWristWidth", attrs), "mm");
	antro->rightWristWidth = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("RightWristWidth", attrs), "mm");

	antro->leftWristThickness = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("LeftWristThickness", attrs), "mm");
	antro->rightWristThickness = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("RightWristThickness", attrs), "mm");

	antro->leftHandWidth = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("LeftHandWidth", attrs), "mm");
	antro->rightHandWidth = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("RightHandWidth", attrs), "mm");

	antro->leftHandThickness = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("LeftHandThickness", attrs), "mm");
	antro->rightHandThickness = hmdbCommunication::AntropometricData::value_type(getAntropometricValue("RightHandThickness", attrs), "mm");

	return antro;
}

const core::VariantPtr HMDBShallowCopyLocalContext::findObjectByType(const DataType type, const hmdbServices::ID id,
	const utils::TypeInfo & typeInfo) const
{
	core::VariantPtr ret;

	auto it = data_.find(type);
	if (it != data_.end()){
		auto IT = it->second.find(id);
		if (IT != it->second.end()){
			for (auto dIT = IT->second.begin(); dIT != IT->second.end(); ++dIT){
				auto ow = *dIT;
				if (ow->data()->isSupported(typeInfo) == true){
					ret = utils::const_pointer_cast<core::Variant>(ow);
					break;
				}
			}
		}			
	}

	return ret;
}

PluginSubject::SubjectPtr HMDBShallowCopyLocalContext::getSubject(const hmdbServices::ID id,
	utils::shared_ptr<PluginSubject::ISubjectService> subjectService,
	const ShallowCopyConstPtr shallowCopy)
{
	core::VariantPtr subOW = findObjectByType(SubjectType, id, typeid(PluginSubject::ISubject));
	
	PluginSubject::SubjectPtr subPtr;

	if (subOW == nullptr){
		//tworzê subjecta
		subOW = subjectService->createSubject();

		//FIX dla linux RtR
		//subPtr = subOW->get();
		subOW->tryGet(subPtr);

		std::stringstream label;

		auto pIT = shallowCopy->medicalShallowCopy.patients.find(id);

		if (pIT == shallowCopy->medicalShallowCopy.patients.end()){
			label << "Subject " << id;
		}
		else{
			label << pIT->second->name << ", " << pIT->second->surname;
			//TODO
			//addPatientObject(pIT->second, subPtr->getID(), transaction.get());
		}

		subOW->setMetadata("label", label.str());

		//dodajê do DM
		localContext_->load(subOW);

		//zapamiêtujê mapowanie
		data_[SubjectType][id].push_back(subOW);
	}
	else{
		//FIX dla linux RtR
		//subPtr = subOW->get();
		subOW->tryGet(subPtr);
	}

	return subPtr;
}

PluginSubject::SessionPtr HMDBShallowCopyLocalContext::getSession(const std::pair<Indexes, MotionFiles> & sessionFiles,
	const IndexedData & loadedFilesData,
	PluginSubject::SubjectPtr subPtr,
	core::VariantPtr subOW,
	const hmdbServices::ID id,
	utils::shared_ptr<PluginSubject::ISubjectService> subjectService,
	const ShallowCopyConstPtr shallowCopy)
{
	core::VariantPtr sOW = findObjectByType(SessionType, id, typeid(PluginSubject::ISession));

	PluginSubject::SessionPtr sPtr;

	if (sOW == nullptr){

		auto sIT = shallowCopy->motionShallowCopy.sessions.find(id);

		if (sIT == shallowCopy->motionShallowCopy.sessions.end()){
			PLUGIN_LOG_DEBUG("Inconsistent subject hierarchy");
			return PluginSubject::SessionPtr();
		}

		auto s = sIT->second;

		//tworzê sesjê
		//generujê zbiór ow dla sesji
		core::ConstVariantsList sessionObjects;
		for (auto fIT = sessionFiles.first.begin(); fIT != sessionFiles.first.end(); ++fIT){
			//pobieram obiekty

			auto it = loadedFilesData.find(*fIT);

			if (it != loadedFilesData.end()){
				sessionObjects.insert(sessionObjects.end(), it->second.begin(), it->second.end());
			}
			else{
				PLUGIN_LOG_DEBUG("Empty file in hierarchy");
			}
		}

		//dane antropometryczne
		auto antro = createAntropometricData(s->performerConf->attrs);
		//tworze OW dla danych antropometrycznych
		auto antroOW = core::Variant::create<hmdbCommunication::AntropometricData>();
		antroOW->set(antro);

		sessionObjects.push_back(antroOW);

		sOW = subjectService->createSession(subOW);

		//FIX dla linux RtR
		//sPtr = sOW->get();
		sOW->tryGet(sPtr);

		for (auto it = sessionObjects.begin(); it != sessionObjects.end(); ++it){
			sPtr->addData(*it);
		}

		sOW->setMetadata("label", s->sessionName);
		sOW->setMetadata("EMGConf", boost::lexical_cast<std::string>(s->emgConf));
		sOW->setMetadata("data", hmdbServices::toString(s->sessionDate));
		if (s->groupAssigment != nullptr){
			sOW->setMetadata("groupID", boost::lexical_cast<std::string>(s->groupAssigment->sessionGroupID));
			auto sgIT = shallowCopy->motionMetaData.sessionGroups.find(s->groupAssigment->sessionGroupID);
			if (sgIT != shallowCopy->motionMetaData.sessionGroups.end()){
				sOW->setMetadata("groupName", sgIT->second.sessionGroupName);
			}
		}

		//dodajê do DM
		localContext_->load(antroOW);
		localContext_->load(sOW);		

		//zapamiêtujê mapowanie
		data_[SessionType][id].push_back(sOW);
		data_[SessionType][id].push_back(antroOW);
		subPtr->addSession(sOW);
	}
	else{
		//FIX dla linux RtR
		//sPtr = sOW->get();
		sOW->tryGet(sPtr);
	}

	return sPtr;
}

const core::VariantPtr createJointsAngles(const core::ConstVariantsList objects, PluginSubject::SessionPtr session)
{
	core::VariantPtr ret;

	//sprawdzamy joint angles - jeœli nie ma budujemy i dodajemy do DM
	core::VariantConstPtr dataWrapper;
	core::VariantConstPtr modelWrapper;
	for (auto it = objects.begin(); it != objects.end(); ++it) {
		if ((*it)->data()->isSupported(typeid(kinematic::JointAnglesCollection))) {
			return ret;
		}
		else if ((*it)->data()->isSupported(typeid(kinematic::SkeletalData))) {
			dataWrapper = *it;
			break;
		}
	}

	core::VariantsCollection modelWrappers(typeid(kinematic::SkeletalModel), false);
	session->getObjects(modelWrappers);	

	if (modelWrappers.empty() == false){
		modelWrapper = modelWrappers.front();

		if (dataWrapper && modelWrapper) {
			ret = core::Variant::create<kinematic::JointAnglesCollection>();
			ret->setInitializer(core::VariantInitializerPtr(new JointsInitializer(dataWrapper, modelWrapper)));
		}
	}

	return ret;
}

PluginSubject::MotionPtr HMDBShallowCopyLocalContext::getMotion(const Indexes & motionFiles,
	const IndexedData & loadedFilesData,
	PluginSubject::SessionPtr sPtr,
	core::VariantPtr sOW,
	const hmdbServices::ID id,
	utils::shared_ptr<PluginSubject::ISubjectService> subjectService,
	const ShallowCopyConstPtr shallowCopy)
{
	core::VariantPtr mOW = findObjectByType(MotionType, id, typeid(PluginSubject::IMotion));

	PluginSubject::MotionPtr mPtr;
	
	if (mOW == nullptr){

		auto mIT = shallowCopy->motionShallowCopy.trials.find(id);

		if (mIT == shallowCopy->motionShallowCopy.trials.end()){
			PLUGIN_LOG_DEBUG("Broken subject hierarchy");
			return mPtr;
		}

		auto m = mIT->second;

		//tworzê sesjê
		//generujê zbiór ow dla motiona
		core::ConstVariantsList motionObjects;
		for (auto fIT = motionFiles.begin(); fIT != motionFiles.end(); ++fIT){

			auto it = loadedFilesData.find(*fIT);

			if (it != loadedFilesData.end()){
				motionObjects.insert(motionObjects.end(), it->second.begin(), it->second.end());
			}
		}

		core::VariantPtr jointsWrapper = createJointsAngles(motionObjects, sPtr);

		if (jointsWrapper != nullptr){
			motionObjects.push_back(jointsWrapper);
		}		

		mOW = subjectService->createMotion(sOW);
		//FIX dla linux RtR
		//mPtr = mOW->get();
		mOW->tryGet(mPtr);

		for (auto it = motionObjects.begin(); it != motionObjects.end(); ++it){
			mPtr->addData(*it);
		}

		if (mPtr->hasObject(typeid(VideoChannel), false) && mPtr->hasObject(typeid(MovieDelays), false)) {
			core::VariantsCollection videoCollection(typeid(VideoChannel), false);
			core::VariantsCollection movieDelaysCollection(typeid(MovieDelays), false);
			mPtr->getObjects(videoCollection);
			mPtr->getObjects(movieDelaysCollection);
			if (movieDelaysCollection.size() == 1) {
				MovieDelaysConstPtr delays = (*(movieDelaysCollection.begin()))->get();
				if (delays->size() == videoCollection.size()) {
					int i = 0;
					for (auto it = videoCollection.begin(); it != videoCollection.end(); ++it) {
						core::VariantPtr wrp = utils::const_pointer_cast<core::Variant>(*it);
						wrp->setMetadata("movieDelay", boost::lexical_cast<std::string>(delays->at(i++)));
					}
				}
				else {
					PLUGIN_LOG_ERROR("Unable to map movie delays");
				}
			}
			else {
				PLUGIN_LOG_ERROR("Wrong movie delays");
			}
		}

		if (jointsWrapper != nullptr){
			//metadane
			jointsWrapper->setMetadata("name", mPtr->getLocalName() + " joints");
			jointsWrapper->setMetadata("source", "hmdbCommunication->motion->" + mPtr->getLocalName());
		}

		mOW->setMetadata("label", m->trialName);

		localContext_->load(mOW);

		//zapamiêtujê mapowanie
		data_[MotionType][id].push_back(mOW);

		if (jointsWrapper != nullptr){

			data_[MotionType][id].push_back(jointsWrapper);
			localContext_->load(jointsWrapper);
		}

		sPtr->addMotion(mOW);
	}
	else{
		//FIX dla linux RtR
		//mPtr = mOW->get();
		mOW->tryGet(mPtr);
	}

	return mPtr;
}

const HMDBShallowCopyLocalContext::SubjectFiles HMDBShallowCopyLocalContext::groupDataInHierarchy(const IndexedData & loadedFilesData,
	const ShallowCopyConstPtr shallowCopy, Indexes & loadedFiles)
{
	SubjectFiles subjectHierarchy;

	auto itEND = loadedFilesData.end();
	for (auto it = loadedFilesData.begin(); it != itEND; ++it){
		loadedFiles.insert(it->first);
		if (it->first < 0){
			auto s = shallowCopy->motionShallowCopy.sessions.find(-it->first);

			if (s != shallowCopy->motionShallowCopy.sessions.end()){
				subjectHierarchy[s->second->performerConf->performer->performerID][s->second->sessionID].first.insert(it->first);
			}
			else{
				//TODO
				//INFO o nieobs³ugiwanym pliku
			}
		}
		else{
			auto fileIT = shallowCopy->motionShallowCopy.files.find(it->first);
			if (fileIT == shallowCopy->motionShallowCopy.files.end()){
				//TODO
				//INFO o nieobs³ugiwanym pliku
				continue;
			}

			if (fileIT->second->isSessionFile() == true){
				subjectHierarchy[fileIT->second->session->performerConf->performer->performerID][fileIT->second->session->sessionID].first.insert(it->first);
			}
			else{
				subjectHierarchy[fileIT->second->trial->session->performerConf->performer->performerID][fileIT->second->trial->session->sessionID].second[fileIT->second->trial->trialID].insert(it->first);
			}
		}
	}

	return subjectHierarchy;
}

void HMDBShallowCopyLocalContext::loadSubjectHierarchy(const IndexedData & loadedFilesData)
{
	auto subjectService = core::queryService<PluginSubject::ISubjectService>(plugin::getServiceManager());

	if (subjectService == nullptr){
		return;
	}

	auto shallowCopy = shallowCopyContext_->shallowCopy();

	//buduje mapê hierarchii subject -> session -> motion -> files
	//na bazie tej mapy bêdê realizowa³ hierarchiê pluginu subject

	Indexes loadedFiles;
	SubjectFiles subjectHierarchy = groupDataInHierarchy(loadedFilesData, shallowCopy, loadedFiles);

	auto transaction = localContext_->transaction();
	//auto hierarchyTransaction = mdm->hierarchyTransaction();
	for (auto subjectIT = subjectHierarchy.begin(); subjectIT != subjectHierarchy.end(); ++subjectIT){
		//tworzê subject jeœli to konieczne!!

		core::VariantPtr subOW;
		PluginSubject::SubjectPtr subPtr = getSubject(subjectIT->first, subjectService, shallowCopy);		

		//mam subjecta, mogê iœæ dalej do sesji
		for (auto sessionIT = subjectIT->second.begin(); sessionIT != subjectIT->second.end(); ++sessionIT){
			
			core::VariantPtr sOW;
			PluginSubject::SessionPtr sPtr = getSession(sessionIT->second,
				loadedFilesData, subPtr, subOW, sessionIT->first,
				subjectService, shallowCopy);

			//mam sesjê - mogê iœæ dalej z motionami!!
			for (auto motionIT = sessionIT->second.second.begin(); motionIT != sessionIT->second.second.end(); ++motionIT){
				core::VariantPtr mOW;
				PluginSubject::MotionPtr mPtr = getMotion(motionIT->second,
					loadedFilesData, sPtr, sOW, motionIT->first,
					subjectService, shallowCopy);
			}
		}

		/*
		std::vector<IHierarchyPerspectivePtr> perspectives = dataSource->getHierarchyPerspectives();
		std::set<core::IHierarchyItemConstPtr> roots;
		for (auto it = perspectives.begin(); it != perspectives.end(); ++it) {
			//auto root = TreeBuilder::createTree("SUB", subPtr);
			auto root = (*it)->getPerspective(subPtr);
			if (root) {
				int childCount = root->getNumChildren();
				for (int c = 0; c < childCount; ++c) {
					auto r = root->getChild(c);
					updateOrAddRoot(r, roots, hierarchyTransaction);
				}
			}
		}
		files2roots[loadedFiles] = roots;
		*/
	}
}

const bool HMDBShallowCopyLocalContext::load(const DataType type,
	const hmdbServices::ID id)
{
	bool ret = false;

	auto files = filesToProcess(type, id, shallowCopyContext_->shallowCopy());

	files = filterFiles(files, shallowCopyContext_->dataStatusManager(), DataStatus(DataStatus::Local, DataStatus::Unloaded));

	if (files.empty() == false){
		auto t = localContext_->transaction();
		IndexedData indexedData;
		for (auto it = files.begin(); it != files.end(); ++it){
			auto fIT = shallowCopyContext_->shallowCopy()->motionShallowCopy.files.find(*it);

			if (fIT != shallowCopyContext_->shallowCopy()->motionShallowCopy.files.end()){
				t->load(fIT->second->fileName);
				core::ConstVariantsList loadedData = t->data(fIT->second->fileName);
				if (loadedData.empty() == false){
					indexedData[*it] = loadedData;
				}
			}			
		}

		if (indexedData.empty() == false){
			loadSubjectHierarchy(indexedData);
		}
		else{
			PLUGIN_LOG_DEBUG("Files without data to load");
		}

		ret = true;
	}
	else{
		PLUGIN_LOG_DEBUG("No files to load");
	}

	return ret;
}

const bool HMDBShallowCopyLocalContext::loadAll()
{
	return load(FileType, -1);
}

const bool HMDBShallowCopyLocalContext::unload(const DataType type,
	const hmdbServices::ID id)
{
	bool ret = false;

	auto files = filesToProcess(type, id, shallowCopyContext_->shallowCopy());

	files = filterFiles(files, shallowCopyContext_->dataStatusManager(), DataStatus(DataStatus::Local, DataStatus::Loaded));

	if (files.empty() == false){
		auto t = localContext_->transaction();
		for (auto it = files.begin(); it != files.end(); ++it){
			auto fIT = shallowCopyContext_->shallowCopy()->motionShallowCopy.files.find(*it);

			if (fIT != shallowCopyContext_->shallowCopy()->motionShallowCopy.files.end()){
				t->unload(fIT->second->fileName);
			}
		}

		unloadSubjectHierarchy(type, id);

		ret = true;
	}
	else{
		PLUGIN_LOG_DEBUG("No files to unload");
	}

	return ret;
}

const bool HMDBShallowCopyLocalContext::unloadAll()
{
	return false;

	if (shallowCopyContext_->shallowCopy() == nullptr){
		return false;
	}

	bool ret = false;

	auto files = filesToProcess(FileType, -1, shallowCopyContext_->shallowCopy());

	files = filterFiles(files, shallowCopyContext_->dataStatusManager(), DataStatus(DataStatus::Local, DataStatus::Loaded));
	auto t = localContext_->transaction();

	if (files.empty() == false){
		for (auto it = files.begin(); it != files.end(); ++it){
			auto fIT = shallowCopyContext_->shallowCopy()->motionShallowCopy.files.find(*it);

			if (fIT != shallowCopyContext_->shallowCopy()->motionShallowCopy.files.end()){
				t->unload(fIT->second->fileName);
			}
		}

		ret = true;
	}
	else{
		PLUGIN_LOG_DEBUG("No files to unload");
	}	

	//po typach
	for (auto it = data_.begin(); it != data_.end(); ++it){
		//po identyfikatorach
		for (auto iIT = it->second.begin(); iIT != it->second.end(); ++iIT){
			//po danych
			for (auto rIT = iIT->second.begin(); rIT != iIT->second.end(); ++rIT){
				t->unload(*rIT);
				ret = true;
			}
		}
	}

	GroupedData().swap(data_);

	return ret;
}

void HMDBShallowCopyLocalContext::unloadSubjectHierarchy(const DataType type,
	const hmdbServices::ID id)
{
	auto ci = childItems(type, id, shallowCopyContext_->shallowCopy());
	ci[type].insert(id);

	auto t = localContext_->transaction();

	//po typach
	for (auto tIT = ci.begin(); tIT != ci.end(); ++tIT){
		auto dIT = data_.find(tIT->first);
		if (dIT != data_.end() && dIT->second.empty() == false){
			//po identyfikatorach do usuniêcia
			for (auto IT = tIT->second.begin(); IT != tIT->second.end(); ++IT){
				//czy takie dane by³y ³adowane
				auto rIT = dIT->second.find(*IT);
				if (rIT != dIT->second.end()){
					//po danych
					for (auto it = rIT->second.begin(); it != rIT->second.end(); ++it){
						t->unload(*it);
					}

					dIT->second.erase(rIT);
				}
			}

			if (dIT->second.empty() == true){
				data_.erase(dIT);
			}
		}
	}
}

/*
{
auto hierarchyTransaction = mdm->hierarchyTransaction();
auto entry = files2roots.find(unloadFiles);
if (entry != files2roots.end()) {
auto roots = entry->second;
for (auto it = roots.begin(); it != roots.end(); ++it) {
hierarchyTransaction->removeRoot(*it);
name2root.erase((*it)->getName());
}

files2roots.erase(entry);
}
else{
auto locFiles2roots = files2roots;

//szukam po rootach czy ich przeciêcie z aktualn¹ grup¹ istnieje
for (auto IT = locFiles2roots.begin(); IT != locFiles2roots.end(); ++IT){
std::vector<int> inter(min(unloadFiles.size(), IT->first.size()));
auto retIT = std::set_intersection(unloadFiles.begin(), unloadFiles.end(), IT->first.begin(), IT->first.end(), inter.begin());
if (std::distance(inter.begin(), retIT) == IT->first.size()){
auto roots = IT->second;
for (auto it = roots.begin(); it != roots.end(); ++it) {
hierarchyTransaction->removeRoot(*it);
name2root.erase((*it)->getName());
}

files2roots.erase(IT->first);
}
}
}
}

return unloadedFiles;
*/

const IHMDBShallowCopyDataContextPtr HMDBShallowCopyLocalContext::shallowCopyContext()
{
	return shallowCopyContext_;
}

const IHMDBShallowCopyDataContextConstPtr HMDBShallowCopyLocalContext::shallowCopyContext() const
{
	return shallowCopyContext_;
}

const IHMDBLocalContextPtr HMDBShallowCopyLocalContext::localContext()
{
	return localContext_;
}

const IHMDBLocalContextConstPtr HMDBShallowCopyLocalContext::localContext() const
{
	return localContext_;
}

HMDBShallowCopyRemoteContext::HMDBShallowCopyRemoteContext(IHMDBShallowCopyDataContextPtr shallowCopyContext,
	IHMDBRemoteContextPtr remoteContext) : shallowCopyContext_(shallowCopyContext),
	remoteContext_(remoteContext)
{

}

HMDBShallowCopyRemoteContext::~HMDBShallowCopyRemoteContext()
{

}

const std::list<IHMDBRemoteContext::DownloadOperationPtr> HMDBShallowCopyRemoteContext::prepareDownload(const DataType type,
	const hmdbServices::ID id, const bool recursive, const bool forceDownload)
{
	return std::list<IHMDBRemoteContext::DownloadOperationPtr>();
}

const HMDBShallowCopyRemoteContext::SynchronizeOperationPtr HMDBShallowCopyRemoteContext::prepareSynchronization(IHMDBStoragePtr storage)
{
	auto sDownloads = remoteContext()->prepareSynchronization();
	return SynchronizeOperationPtr(new hmdbCommunication::SynchronizeOperation(sDownloads, storage));
}

void updateIncrementalShallowCopyStatus(const hmdbServices::IncrementalBranchShallowCopy::ShallowCopy & ishallowCopy,
	const DataStatus & dataStatus, IHMDBStatusManager::TransactionPtr transaction)
{
	for (auto it = ishallowCopy.files.begin(); it != ishallowCopy.files.end(); ++it){
		transaction->updateDataStatus(FileType, (*it).fileID, dataStatus);
	}

	for (auto it = ishallowCopy.trials.begin(); it != ishallowCopy.trials.end(); ++it){
		transaction->updateDataStatus(MotionType, (*it).trialID, dataStatus);
	}

	for (auto it = ishallowCopy.sessions.begin(); it != ishallowCopy.sessions.end(); ++it){
		transaction->updateDataStatus(SessionType, (*it).sessionID, dataStatus);
	}

	for (auto it = ishallowCopy.performers.begin(); it != ishallowCopy.performers.end(); ++it){
		transaction->updateDataStatus(SubjectType, (*it).performerID, dataStatus);
	}
}

void HMDBShallowCopyRemoteContext::synchronize(const ShallowCopyConstPtr shallowCopy,
	const IncrementalBranchShallowCopyConstPtr incrementalBranchShallowCopy)
{
	shallowCopyContext_->setShallowCopy(shallowCopy);
	if (incrementalBranchShallowCopy != nullptr){
		auto t = shallowCopyContext_->dataStatusManager()->transaction();
		updateIncrementalShallowCopyStatus(incrementalBranchShallowCopy->added, DataStatus(DataStatus::Remote, DataStatus::Unloaded, DataStatus::Valid), t);
		updateIncrementalShallowCopyStatus(incrementalBranchShallowCopy->modified, DataStatus(DataStatus::Outdated), t);
		//TODO
		//co robic z takimi danymi?
		updateIncrementalShallowCopyStatus(incrementalBranchShallowCopy->removedLocaly, DataStatus(DataStatus::Outdated), t);
		updateIncrementalShallowCopyStatus(incrementalBranchShallowCopy->removedGlobaly, DataStatus(DataStatus::Outdated), t);
	}
}

void HMDBShallowCopyRemoteContext::synchronize(const SynchronizeOperationPtr downloadOperations)
{
	downloadOperations->start();
	downloadOperations->wait();
	if (downloadOperations->status() == threadingUtils::IOperation::Finished){
		auto sc = downloadOperations->shallowCopy();
		auto isc = downloadOperations->incrementalBranchShallowCopy();
		synchronize(sc, isc);
	}
}

const IHMDBShallowCopyDataContextPtr HMDBShallowCopyRemoteContext::shallowCopyContext()
{
	return shallowCopyContext_;
}

const IHMDBShallowCopyDataContextConstPtr HMDBShallowCopyRemoteContext::shallowCopyContext() const
{
	return shallowCopyContext_;
}

const IHMDBRemoteContextPtr HMDBShallowCopyRemoteContext::remoteContext()
{
	return remoteContext_;
}

const IHMDBRemoteContextConstPtr HMDBShallowCopyRemoteContext::remoteContext() const
{
	return remoteContext_;
}

HMDBShallowCopyContext::HMDBShallowCopyContext(IHMDBShallowCopyDataContextPtr scdc,
	IHMDBShallowCopyLocalContextPtr sclc,
	IHMDBShallowCopyRemoteContextPtr scrc)
	: dataContext_(scdc), localContext_(sclc), remoteContext_(scrc)
{

}

HMDBShallowCopyContext::~HMDBShallowCopyContext()
{

}

const IHMDBShallowCopyDataContextPtr HMDBShallowCopyContext::shallowCopyDataContext()
{
	return dataContext_;
}

const IHMDBShallowCopyDataContextConstPtr HMDBShallowCopyContext::shallowCopyDataContext() const
{
	return dataContext_;
}

const IHMDBShallowCopyLocalContextPtr HMDBShallowCopyContext::shallowCopyLocalContext()
{
	return localContext_;
}

const IHMDBShallowCopyLocalContextConstPtr HMDBShallowCopyContext::shallowCopyLocalContext() const
{
	return localContext_;
}

const IHMDBShallowCopyRemoteContextPtr HMDBShallowCopyContext::shallowCopyRemoteContext()
{
	return remoteContext_;
}

const IHMDBShallowCopyRemoteContextConstPtr HMDBShallowCopyContext::shallowCopyRemoteContext() const
{
	return remoteContext_;
}