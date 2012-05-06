#include "CommunicationPCH.h"
#include "DataSourcePathsManager.h"
#include <OpenThreads/Mutex>
#include <OpenThreads/ScopedLock>
#include <cctype>

DataSourcePathsManager * DataSourcePathsManager::instance_ = nullptr;

DataSourcePathsManager * DataSourcePathsManager::create(const core::Filesystem::Path & usersDataPath)
{
	if(instance_ != nullptr){
		throw std::runtime_error("Instance already exist");
	}

	instance_ = new DataSourcePathsManager(usersDataPath);
}

void DataSourcePathsManager::destroy()
{
	if(instance_ != nullptr){
		delete instance_;
		instance_ = nullptr;
	}
}

DataSourcePathsManager * DataSourcePathsManager::instance()
{
	return instance_;
}

DataSourcePathsManager::DataSourcePathsManager(const core::Filesystem::Path & usersDataPath)
	: localUsersDataPath(usersDataPath)
{
	
}

DataSourcePathsManager::~DataSourcePathsManager()
{

}

void DataSourcePathsManager::setUser(const User & user)
{
	if(user_.id() != -1){
		clearUserPaths();
	}

	user_ = user;

	if(user_.id() != -1){
		userHash_ = generateUserHash(user);
		rebuildUserPaths();
	}
}

void DataSourcePathsManager::setUsersDataPath(const core::Filesystem::Path & usersDataPath)
{
	//Aktualizuje g³ówn¹ œcie¿kê danych
	localUsersDataPath = usersDataPath;

	//Muszê aktualizowaæ œcie¿ki
	if(user_.id() != -1){
		rebuildUserPaths();
	}
}

const core::Filesystem::Path & DataSourcePathsManager::userDataPath() const
{
	return localUserDataPath;
}

const core::Filesystem::Path & DataSourcePathsManager::usersDataPath() const
{
	return localUsersDataPath;
}

const std::string & DataSourcePathsManager::userHash() const
{
	return userHash_;
}

std::string DataSourcePathsManager::generateUserHash(const User & user)
{
	//TODO
	//dodaæ coœ sensownie mieszaj¹cego
	std::string hash(user.name() + ":" + user.password());
	
	bool first = true;

	for(int i = 0; i < hash.size(); ++i){
		unsigned char c = (hash[i] + 5) % 128; 
		if(std::isalnum(c) == false){
			char nChar = c / 43;

			if(first == true){
				nChar = 'a' + c % 24;
				first = false;
			}else{

				switch(nChar){
				case 1:

					//obcinamy do cyfr
					nChar = '0' + c % 9;
					break;

				case 2:

					//obcinamy do dy¿ych liter
					nChar = 'A' + c % 24;
					break;

				case 0:

					//obcinamy do ma³ych liter
					nChar = 'a' + c % 24;
					break;
				}
			}

			hash[i] = nChar;
		}else{
			hash[i] = c;
		}
	}

	return hash;
}

const core::Filesystem::Path & DataSourcePathsManager::motionDataPath() const
{
	return localDataPath;
}

const core::Filesystem::Path & DataSourcePathsManager::schemasPath() const
{
	return localSchemasPath;
}

const core::Filesystem::Path & DataSourcePathsManager::photosPath() const
{
	return localPatientPhotosPath;
}

const core::Filesystem::Path DataSourcePathsManager::sessionPath(const std::string & sessionName) const
{
	return localDataPath / sessionName;
}

const core::Filesystem::Path DataSourcePathsManager::filePath(const std::string & fileName, const std::string & sessionName) const
{
	return localDataPath / sessionName / fileName;
}

const core::Filesystem::Path DataSourcePathsManager::photoPath(const std::string & photoname) const
{
	return localPatientPhotosPath / photoname;
}

bool DataSourcePathsManager::pathExists(const core::Filesystem::Path & path)
{
	return core::Filesystem::pathExists(path);
}

const core::Filesystem::Path & DataSourcePathsManager::projectsPath() const
{
	return localProjectsPath;
}

const core::Filesystem::Path & DataSourcePathsManager::motionShallowCopyPath() const
{
	return localMotionShallowCopyPath;
}

const core::Filesystem::Path & DataSourcePathsManager::motionMetadataPath() const
{
	return localMotionMetadataPath;
}

const core::Filesystem::Path & DataSourcePathsManager::medicalShallowCopyPath() const
{
	return localMedicalShallowCopyPath;
}

const core::Filesystem::Path & DataSourcePathsManager::medicalMetadataPath() const
{
	return localMedicalMetadataPath;
}

unsigned long long DataSourcePathsManager::freeSpace(const core::Filesystem::Path & path)
{
	return boost::filesystem::space(path).free;
}

void DataSourcePathsManager::rebuildUserPaths()
{
	localUserDataPath = localUsersDataPath / userHash_;

	localDataPath = localUserDataPath / "data";
	localSchemasPath = localUserDataPath / "scheme";
	localProjectsPath = localDataPath / (userHash_ + "projects.xml");
	localMotionSchemasPath = localSchemasPath / "motion";
	
	localMedicalSchemasPath = localSchemasPath / "medical";
	
	localPatientPhotosPath = localMedicalSchemasPath / "patient_photos";
	
	// dodajê na pocz¹tek m dla unikalnoœci (od motion) oraz s na koniec (od shallow)
	localMotionShallowCopyPath = localMotionSchemasPath / ("m" + userHash_ + "s.xml");
	// dodajê na pocz¹tek m dla unikalnoœci (od motion) oraz m na koniec (od metadata)
	localMotionMetadataPath = localMotionSchemasPath / ("m" + userHash_ + "m.xml");
	// dodajê na pocz¹tek med dla unikalnoœci (od medical) oraz s na koniec (od shallow)
	localMedicalShallowCopyPath = localMedicalSchemasPath / ("med" + userHash_ + "s.xml");
	// dodajê na pocz¹tek med dla unikalnoœci (od medical) oraz m na koniec (od metadata)
	localMedicalMetadataPath = localMedicalSchemasPath / ("med" + userHash_ + "m.xml");
}

void DataSourcePathsManager::clearUserPaths()
{
	localUserDataPath = localUsersDataPath;
	localDataPath = localUsersDataPath;
	localSchemasPath = localUsersDataPath;
	localMotionSchemasPath = localUsersDataPath;
	localMedicalSchemasPath = localUsersDataPath;
	localPatientPhotosPath = localUsersDataPath;
	localMotionShallowCopyPath = localUsersDataPath;
	localMotionMetadataPath = localUsersDataPath;
	localMedicalShallowCopyPath = localUsersDataPath;
	localMedicalMetadataPath = localUsersDataPath;
}

void DataSourcePathsManager::createUserDataPaths() const
{
	std::vector<core::Filesystem::Path> paths;
	
	try{
		core::Filesystem::createDirectory(localUsersDataPath);
		paths.push_back(localUsersDataPath);

		core::Filesystem::createDirectory(localUserDataPath);
		paths.push_back(localUserDataPath);

		core::Filesystem::createDirectory(localDataPath);
		paths.push_back(localDataPath);

		core::Filesystem::createDirectory(localSchemasPath);
		paths.push_back(localSchemasPath);

		core::Filesystem::createDirectory(localMotionSchemasPath);
		paths.push_back(localMotionSchemasPath);

		core::Filesystem::createDirectory(localMedicalSchemasPath);
		paths.push_back(localMedicalSchemasPath);

		core::Filesystem::createDirectory(localPatientPhotosPath);
		paths.push_back(localPatientPhotosPath);
	}catch(...){
		try{
			for(auto it = paths.rbegin(); it != paths.rend(); ++it){
				core::Filesystem::deleteDirectory(*it);
			}
		}catch(...){

		}

		throw;
	}
}

void DataSourcePathsManager::removeUserDataPaths() const
{
	// próbujemy posprz¹taæ po u¿ytkowniku jego dane usuwaj¹c ich roota
	try{
		core::Filesystem::deleteDirectory(localUserDataPath);
	}
	catch(...){

	}
}