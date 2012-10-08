#include "CommunicationPCH.h"
#include "DataSourceStatusManager.h"
#include <QtGui/QPainter>

using namespace communication;

static const DataStatus partialStatus = DataStatus(PartiallyLocal, PartiallyLoaded);
std::map<communication::DataStorage, std::map<communication::DataUsage, QIcon>> DataSourceStatusManager::statusIcons = std::map<communication::DataStorage, std::map<communication::DataUsage, QIcon>>();

FileStatusManager::FileStatusManager(const core::IFileDataManager * fileDataManager) : fileDataManager(fileDataManager)
{
    if(fileDataManager == nullptr){
        throw std::invalid_argument("Uninitialized core::IFileDataManager object");
    }
}

FileStatusManager::~FileStatusManager()
{

}

void FileStatusManager::addFile(int fileID, const core::Filesystem::Path & path, const DataStatus & status)
{
    auto it = filesStatus.find(fileID);
    if(it != filesStatus.end()){
        throw std::runtime_error("File already registered");
    }
    FileStatus fStatus = {fileID, path, status};
    filesStatus.insert(FilesStatus::value_type(fileID, fStatus));
	files.insert(fileID);
}

void FileStatusManager::removeFile(int fileID)
{
    auto it = filesStatus.find(fileID);
    if(it == filesStatus.end()){
        throw std::runtime_error("File not registered");
    }

    filesStatus.erase(it);
	files.erase(fileID);
}

void FileStatusManager::removeAllFiles()
{
	FilesStatus().swap(filesStatus);
	std::set<int>().swap(files);
}

bool FileStatusManager::fileExists(int fileID)
{
	return files.find(fileID) != files.end();
}

void FileStatusManager::managedFiles(std::set<int> & files) const
{
	files.insert(this->files.begin(), this->files.end());
}

void FileStatusManager::setFileStatus(int fileID, const DataStatus & status)
{
    auto it = filesStatus.find(fileID);
    if(it == filesStatus.end()){
        throw std::runtime_error("File not registered");
    }

    it->second.fileStatus = status;
}

void FileStatusManager::setFileUsage(int fileID, DataUsage usage)
{
    auto it = filesStatus.find(fileID);
    if(it == filesStatus.end()){
        throw std::runtime_error("File not registered");
    }

    it->second.fileStatus.setUsage( usage );
}

void FileStatusManager::setFileStorage(int fileID, DataStorage storage)
{
    auto it = filesStatus.find(fileID);
    if(it == filesStatus.end()){
        throw std::runtime_error("File not registered");
    }

    it->second.fileStatus.setStorage( storage );
}

void FileStatusManager::setFilePath(int fileID, const core::Filesystem::Path & path)
{
	auto it = filesStatus.find(fileID);
	if(it == filesStatus.end()){
		throw std::runtime_error("File not registered");
	}

	it->second.filePath = path;
}

const core::Filesystem::Path & FileStatusManager::filePath(int fileID) const
{
    auto it = filesStatus.find(fileID);
    if(it == filesStatus.end()){
        throw std::runtime_error("File not registered");
    }

    return it->second.filePath;
}

const DataStatus & FileStatusManager::fileStatus(int fileID) const
{
    auto it = filesStatus.find(fileID);
    if(it == filesStatus.end()){
        throw std::runtime_error("File not registered");
    }

    return it->second.fileStatus;
}

const DataUsage FileStatusManager::fileUsage(int fileID) const
{
    auto it = filesStatus.find(fileID);
    if(it == filesStatus.end()){
        throw std::runtime_error("File not registered");
    }

    return it->second.fileStatus.usage();
}

const DataStorage FileStatusManager::fileStorage(int fileID) const
{
    auto it = filesStatus.find(fileID);
    if(it == filesStatus.end()){
        throw std::runtime_error("File not registered");
    }

    return it->second.fileStatus.storage();
}

void FileStatusManager::refreshFilesStatus(const std::set<int> & files)
{
    core::Files managedFiles;
    fileDataManager->getManagedFiles(managedFiles);

    auto managedFilesITEnd = managedFiles.end();

    std::map<FileStatus*, DataStatus> newStatus;

    auto itEnd = files.end();
    auto fileITEnd = filesStatus.end();
    for(auto it = files.begin(); it != itEnd; ++it){
        auto fileIT = filesStatus.find(*it);
        if(fileIT == fileITEnd){
            throw std::runtime_error("File not registered");
        }

        newStatus.insert(std::map<FileStatus*, DataStatus>::value_type(&(fileIT->second),
            DataStatus(core::Filesystem::pathExists(fileIT->second.filePath) == true ? Local : Remote,
            managedFiles.find(fileIT->second.filePath) != managedFilesITEnd ? Loaded : Unloaded)));
    }

    auto newStatusITEnd = newStatus.end();

    for(auto it = newStatus.begin(); it != newStatusITEnd; ++ it){
        it->first->fileStatus = it->second;
    }
}

void FileStatusManager::refreshFilesStatus()
{
    core::Files managedFiles;
    fileDataManager->getManagedFiles(managedFiles);

    auto managedFilesITEnd = managedFiles.end();

    std::map<FileStatus*, DataStatus> newStatus;

    for(auto it = filesStatus.begin(); it != filesStatus.end(); ++it){
        newStatus.insert(std::map<FileStatus*, DataStatus>::value_type(&(it->second),
            DataStatus(core::Filesystem::pathExists(it->second.filePath) == true ? Local : Remote,
            managedFiles.find(it->second.filePath) != managedFilesITEnd ? Loaded : Unloaded)));
    }

    auto newStatusITEnd = newStatus.end();

    for(auto it = newStatus.begin(); it != newStatusITEnd; ++ it){
        it->first->fileStatus = it->second;
    }
}

void FileStatusManager::refreshFileStatus(int fileID)
{
    auto it = filesStatus.find(fileID);
    if(it == filesStatus.end()){
        throw std::runtime_error("File not registered");
    }

    refreshFileStatus(it->second);
}

void FileStatusManager::refreshFileStatus(FileStatus & fileStatus)
{
    DataStorage storage(core::Filesystem::pathExists(fileStatus.filePath) == true ? Local : Remote);
    DataUsage usage(fileDataManager->isFileManaged(fileStatus.filePath) == true ? Loaded : Unloaded);

    fileStatus.fileStatus.setStorage(storage);
    fileStatus.fileStatus.setUsage(usage);
}

DataSourceStatusManager::DataSourceStatusManager(const FileStatusManager * fileStatusManager) : shallowCopy(nullptr), fileStatusManager(fileStatusManager)
{

}

DataSourceStatusManager::~DataSourceStatusManager()
{

}

void DataSourceStatusManager::setShallowCopy(const ShallowCopy * shallowCopy)
{
    clearAllStatusData();

    this->shallowCopy = shallowCopy;

    if(shallowCopy != nullptr){
        rebuildDataStatus();
    }
}

void DataSourceStatusManager::refreshDataStatus(const std::set<int> & modifiedFilesStatus)
{
    //do tego potrzebujemy ShallowCopy
    //znajdź wspólne motiony do odświeżenia
    //znajdź wspólne sesje do odświeżenia
    //znajdź wsólnych subjectów do odświeżenia
    //znajdź wspólnych pacjentów do odświeżenia
    //znajdź wspólne schorzenia do odświeżenia

    std::map<int, const webservices::MedicalShallowCopy::Disorder*> disordersToUpdate;
    std::map<int, const webservices::MedicalShallowCopy::Patient*> patientsToUpdate;
    std::map<int, const webservices::MotionShallowCopy::Performer*> subjectsToUpdate;
    std::map<int, const webservices::MotionShallowCopy::Session*> sesionsToUpdate;
    std::map<int, const webservices::MotionShallowCopy::Trial*> motionsToUpdate;

    auto fITEnd = shallowCopy->motionShallowCopy->files.end();
    auto fileITEnd = modifiedFilesStatus.end();
    for(auto fileIT = modifiedFilesStatus.begin(); fileIT != fileITEnd; ++fileIT){
        auto fIT = shallowCopy->motionShallowCopy->files.find(*fileIT);
        if(fIT == fITEnd){
            //throw std::runtime_error("Unrecognized file");
			continue;
			//TODO
			//warning - file outside from shallowcopy
        }

        if(fIT->second->isSessionFile() == true){
            sesionsToUpdate[fIT->second->session->sessionID] = fIT->second->session;
            subjectsToUpdate[fIT->second->session->performerConf->performer->performerID] = fIT->second->session->performerConf->performer;
        }else{
            motionsToUpdate[fIT->second->trial->trialID] = fIT->second->trial;
            sesionsToUpdate[fIT->second->trial->session->sessionID] = fIT->second->trial->session;
            subjectsToUpdate[fIT->second->trial->session->performerConf->performer->performerID] = fIT->second->trial->session->performerConf->performer;
			if(fIT->second->trial->session->performerConf->performer->patient != nullptr){
				patientsToUpdate[fIT->second->trial->session->performerConf->performer->patient->patientID] = fIT->second->trial->session->performerConf->performer->patient;

				auto disorderITEnd = fIT->second->trial->session->performerConf->performer->patient->disorders.end();
				for(auto disorderIT = fIT->second->trial->session->performerConf->performer->patient->disorders.begin(); disorderIT != disorderITEnd; ++disorderIT){
					disordersToUpdate[disorderIT->second.disorder->disorderID] = disorderIT->second.disorder;
				}
			}
        }
    }

    std::map<int, DataStatus> locDisordersStatus;
    std::map<int, DataStatus> locPatientsStatus;
    std::map<int, DataStatus> locSubjectsStatus;
    std::map<int, DataStatus> locSessionsStatus;
    std::map<int, DataStatus> locMotionsStatus;

    auto motionITEnd = motionsToUpdate.end();
    for(auto motionIT = motionsToUpdate.begin(); motionIT != motionITEnd; ++motionIT){
        DataStatus motionStatus;

        //pliki motiona
        auto fileITEnd = motionIT->second->files.end();
        for(auto fileIT = motionIT->second->files.begin(); fileIT != fileITEnd; ++fileIT){
            motionStatus |= fileStatusManager->fileStatus(fileIT->second->fileID);

			if(partialStatus == motionStatus){
				break;
			}
        }

        locMotionsStatus.insert(std::map<int, DataStatus>::value_type(motionIT->second->trialID, motionStatus));
    }

    auto sessionITEnd = sesionsToUpdate.end();
    for(auto sessionIT = sesionsToUpdate.begin(); sessionIT != sessionITEnd; ++sessionIT){
        DataStatus sessionStatus;

        //pliki motiona
        auto fileITEnd = sessionIT->second->files.end();
        for(auto fileIT = sessionIT->second->files.begin(); fileIT != fileITEnd; ++fileIT){
            sessionStatus |= fileStatusManager->fileStatus(fileIT->second->fileID);
        }

		if(partialStatus != sessionStatus){

			auto motionITEnd = sessionIT->second->trials.end();
			for(auto motionIT = sessionIT->second->trials.begin(); motionIT != motionITEnd; ++motionIT){
				auto locMotionIT = locMotionsStatus.find(motionIT->first);
				if(locMotionIT == locMotionsStatus.end()){
					sessionStatus |= motionsStatus[motionIT->first];
				}else{
					sessionStatus |= locMotionIT->second;
				}

				if(partialStatus == sessionStatus){
					break;
				}
			}
		}

        locSessionsStatus.insert(std::map<int, DataStatus>::value_type(sessionIT->first, sessionStatus));
    }

    auto subjectsITEnd = subjectsToUpdate.end();
    for(auto subjectIT = subjectsToUpdate.begin(); subjectIT != subjectsITEnd; ++subjectIT){
        DataStatus subjectStatus;

        auto confITEnd = subjectIT->second->performerConfs.end();
        for(auto confIT = subjectIT->second->performerConfs.begin(); confIT != confITEnd; ++confIT){
            auto locSessionIT = locSessionsStatus.find(confIT->second->session->sessionID);
            if(locSessionIT == locSessionsStatus.end()){
                subjectStatus |= sessionsStatus[confIT->second->session->sessionID];
            }else{
                subjectStatus |= locSessionIT->second;
            }

			if(partialStatus == subjectStatus){
				break;
			}
        }

        locSubjectsStatus.insert(std::map<int, DataStatus>::value_type(subjectIT->second->performerID, subjectStatus));

		//teraz tak to robimy, jeśli pacjent będzie miał swoje pliki to trzeba to rozbić
		if(subjectIT->second->patient != nullptr){
			locPatientsStatus.insert(std::map<int, DataStatus>::value_type(subjectIT->second->patient->patientID, subjectStatus));
		}
    }

    auto disorderITEnd = disordersToUpdate.end();
    for(auto disorderIT = disordersToUpdate.begin(); disorderIT != disorderITEnd; ++disorderIT){
        DataStatus disorderStatus;

        auto patientsIT = shallowCopy->medicalShallowCopy->patientsByDisorder.find(disorderIT->second->disorderID);
        if(patientsIT != shallowCopy->medicalShallowCopy->patientsByDisorder.end()){
            auto patientITEnd = patientsIT->second.end();
            for(auto patientIT = patientsIT->second.begin(); patientIT != patientITEnd; ++patientIT){
                auto locPatientIT = locPatientsStatus.find((*patientIT)->patientID);
                if(locPatientIT == locPatientsStatus.end()){
                    disorderStatus |= patientsStatus[(*patientIT)->patientID];
                }else{
                    disorderStatus |= locPatientIT->second;
                }

				if(partialStatus == disorderStatus){
					break;
				}
            }
        }

        locDisordersStatus.insert(std::map<int, DataStatus>::value_type(disorderIT->second->disorderID, disorderStatus));
    }

    //przepisanie zaktualizowanych danych
    auto itEnd = locMotionsStatus.end();
    for(auto it = locMotionsStatus.begin(); it != itEnd; ++it){
        motionsStatus[it->first] = it->second;
    }

    itEnd = locSessionsStatus.end();
    for(auto it = locSessionsStatus.begin(); it != itEnd; ++it){
        sessionsStatus[it->first] = it->second;
    }

    itEnd = locSubjectsStatus.end();
    for(auto it = locSubjectsStatus.begin(); it != itEnd; ++it){
        subjectsStatus[it->first] = it->second;
    }

    itEnd = locPatientsStatus.end();
    for(auto it = locPatientsStatus.begin(); it != itEnd; ++it){
        patientsStatus[it->first] = it->second;
    }

    itEnd = locDisordersStatus.end();
    for(auto it = locDisordersStatus.begin(); it != itEnd; ++it){
        disordersStatus[it->first] = it->second;
    }
}

const DataStatus & DataSourceStatusManager::disorderStatus(int disorderID) const
{
    auto it = disordersStatus.find(disorderID);
    if(it == disordersStatus.end()){
        throw std::runtime_error("Disorder not registered");
    }

    return it->second;
}

const DataStatus & DataSourceStatusManager::patientStatus(int patientID) const
{
    auto it = patientsStatus.find(patientID);
    if(it == patientsStatus.end()){
        throw std::runtime_error("Patient not registered");
    }

    return it->second;
}

const DataStatus & DataSourceStatusManager::subjectStatus(int subjectID) const
{
    auto it = subjectsStatus.find(subjectID);
    if(it == subjectsStatus.end()){
        throw std::runtime_error("Subject not registered");
    }

    return it->second;
}

const DataStatus & DataSourceStatusManager::sessionStatus(int sessionID) const
{
    auto it = sessionsStatus.find(sessionID);
    if(it == sessionsStatus.end()){
        throw std::runtime_error("Session not registered");
    }

    return it->second;
}

const DataStatus & DataSourceStatusManager::motionStatus(int motionID) const
{
    auto it = motionsStatus.find(motionID);
    if(it == motionsStatus.end()){
        throw std::runtime_error("Motion not registered");
    }

    return it->second;
}

const DataStatus & DataSourceStatusManager::fileStatus(int fileID) const
{
    auto it = shallowCopy->motionShallowCopy->files.find(fileID);
    if(it == shallowCopy->motionShallowCopy->files.end()){
        throw std::runtime_error("File not registered");
    }

    return fileStatusManager->fileStatus(fileID);
}

void DataSourceStatusManager::rebuildDataStatus()
{
    //lokalne statusy, które potem przepiszemy
    std::map<int, DataStatus> locDisordersStatus;
    std::map<int, DataStatus> locPatientsStatus;
    std::map<int, DataStatus> locSubjectsStatus;
    std::map<int, DataStatus> locSessionsStatus;
    std::map<int, DataStatus> locMotionsStatus;

    auto subjectITEnd = shallowCopy->motionShallowCopy->performers.end();
    for(auto subjectIT = shallowCopy->motionShallowCopy->performers.begin(); subjectIT != subjectITEnd; ++subjectIT){
        DataStatus subjectStatus;

        //sesje subjecta
        auto confITEnd = subjectIT->second->performerConfs.end();
        for(auto confIT = subjectIT->second->performerConfs.begin(); confIT != confITEnd; ++confIT){
            DataStatus sessionStatus;

            //motiony sesji
            auto motionITEnd = confIT->second->session->trials.end();
            for(auto motionIT = confIT->second->session->trials.begin(); motionIT != motionITEnd; ++motionIT){
                DataStatus motionStatus;

                //pliki motiona
                auto fileITEnd = motionIT->second->files.end();
                for(auto fileIT = motionIT->second->files.begin(); fileIT != fileITEnd; ++fileIT){
                    motionStatus |= fileStatusManager->fileStatus(fileIT->second->fileID);
                }

                sessionStatus |= motionStatus;

                locMotionsStatus.insert(std::map<int, DataStatus>::value_type(motionIT->second->trialID, motionStatus));
            }

            //pliki sesji
            auto fileITEnd = confIT->second->session->files.end();
            for(auto fileIT = confIT->second->session->files.begin(); fileIT != fileITEnd; ++fileIT){
                sessionStatus |= fileStatusManager->fileStatus(fileIT->second->fileID);
            }

            subjectStatus |= sessionStatus;

            locSessionsStatus.insert(std::map<int, DataStatus>::value_type(confIT->second->session->sessionID, sessionStatus));
        }

        locSubjectsStatus.insert(std::map<int, DataStatus>::value_type(subjectIT->second->performerID, subjectStatus));
    }

    //pacjenci - aktualnie status pacjenta = status odpowiedniego subjecta
    auto patientITEnd = shallowCopy->medicalShallowCopy->patients.end();
    for(auto patientIT = shallowCopy->medicalShallowCopy->patients.begin(); patientIT != patientITEnd; ++patientIT){
        auto subjectIT = locSubjectsStatus.find(patientIT->second->motionPerformerID);
        if(subjectIT == locSubjectsStatus.end()){
            throw std::runtime_error("Incomplete data - missing subject for patient");
        }

        locPatientsStatus.insert(std::map<int, DataStatus>::value_type(patientIT->second->patientID, subjectIT->second));
    }

    //schorzenia
    auto disorderITEnd = shallowCopy->medicalShallowCopy->patientsByDisorder.end();
    for(auto disorderIT = shallowCopy->medicalShallowCopy->patientsByDisorder.begin(); disorderIT != disorderITEnd; ++disorderIT){
        DataStatus disorderStatus;

        auto patientITEnd = disorderIT->second.end();
        for(auto patientIT = disorderIT->second.begin(); patientIT != patientITEnd; ++ patientIT){
            auto pIT = locPatientsStatus.find((*patientIT)->patientID);
            if(pIT == locPatientsStatus.end()){
                throw std::runtime_error("Incomplete data - missing patient for disorder");
            }

            disorderStatus |= pIT->second;
        }

        locDisordersStatus.insert(std::map<int, DataStatus>::value_type(disorderIT->first, disorderStatus));
    }

    disordersStatus.swap(locDisordersStatus);
    patientsStatus.swap(locPatientsStatus);
    subjectsStatus.swap(locSubjectsStatus);
    sessionsStatus.swap(locSessionsStatus);
    motionsStatus.swap(locMotionsStatus);
}

void DataSourceStatusManager::clearAllStatusData()
{
    std::map<int, DataStatus>().swap(disordersStatus);
    std::map<int, DataStatus>().swap(patientsStatus);
    std::map<int, DataStatus>().swap(subjectsStatus);
    std::map<int, DataStatus>().swap(sessionsStatus);
    std::map<int, DataStatus>().swap(motionsStatus);
}

QIcon DataSourceStatusManager::statusIcon(const communication::IDataStatus & status)
{
	auto storageIT = statusIcons.find(status.storage());
	if(storageIT != statusIcons.end()){
		auto usageIT = storageIT->second.find(status.usage());
		if(usageIT != storageIT->second.end()){
			return usageIT->second;
		}
	}

	return QIcon();
}

void DataSourceStatusManager::setStatusIcon(const communication::IDataStatus & status, const QIcon & icon)
{
	statusIcons[status.storage()][status.usage()] = icon;
}
