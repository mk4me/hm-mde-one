#include "CommunicationPCH.h"
#include <corelib/ILog.h>
#include <corelib/PluginCommon.h>
#include "DataSource.h"
#include <quazip/quazip.h>
#include <quazip/quazipfile.h>
#include <QtCore/QFile>
#include <wsdlparser/WsdlExtension.h>

#include "CommunicationManager.h"
#include <plugins/subject/ISubjectService.h>
#include <plugins/subject/ISubject.h>
#include <plugins/subject/ISession.h>
#include <plugins/subject/IMotion.h>

#include "DataSourceLoginManager.h"
#include "DataSourceStatusManager.h"
#include "DataSourceLocalStorage.h"
#include "DataSourcePathsManager.h"
#include "DataSourceUser.h"
#include "DownloadRequest.h"
#include "Patient.h"

#include "DataSourceConnectionsManager.h"
#include "DataSourceWebServicesManager.h"
#include "DataSourceWidget.h"
#include "DataSourceShallowCopyUtils.h"

using namespace communication;
using namespace webservices;

CommunicationDataSource::CommunicationDataSource() : loginManager(new DataSourceLoginManager()),
	memoryDM(nullptr), fileDM(nullptr),	offlineMode_(false),
	serwerPingUrl("http://v21.pjwstk.edu.pl/"), dataSourceWidget(nullptr)
{    
#if (defined _WIN32) || (defined _WIN64)
	//konfiguracja wsdlpulla żeby pisać pliki tymczasowe tam gdzie mamy prawo zapisu
	//pod windows
	XmlUtils::TMPFILESDIR = plugin::getPathInterface()->getTmpPath().string();
    // rev - zeby to przeszlo, w nagłówku musial być wywalony warunek na WIN32
    WsdlPull::SCHEMADIR = (plugin::getPathInterface()->getResourcesPath() / "schemas/").string();
#endif
    WsdlPull::WsdlParser::useLocalSchema_ = false;
    //konfigurujemy ustawienia połączeń - adresy serwisów i ftp
    auto connectionsManager = DataSourceConnectionManager::create();

    setConnectionsSerwerCertificatePath(plugin::getPathInterface()->getResourcesPath() / "v21.pjwstk.edu.pl.crt");

	connectionsManager->accountFactoryWSConnection()->setUrl("https://v21.pjwstk.edu.pl/HMDBMed/AccountFactoryWS.svc?wsdl");
    connectionsManager->administrationWSConnection()->setUrl("https://v21.pjwstk.edu.pl/HMDB/AdministrationWS.svc?wsdl");
    connectionsManager->authorizationWSConnection()->setUrl("https://v21.pjwstk.edu.pl/HMDB/AuthorizationWS.svc?wsdl");
    connectionsManager->userPersonalSpaceWSConnection()->setUrl("https://v21.pjwstk.edu.pl/HMDB/res/UserPersonalSpaceWSStandalone.wsdl");

    connectionsManager->motionBasicQueriesWSConnection()->setUrl("https://v21.pjwstk.edu.pl/HMDB/res/BasicQueriesWSStandalone.wsdl");
    connectionsManager->motionBasicUpdatesWSConnection()->setUrl("https://v21.pjwstk.edu.pl/HMDB/BasicUpdatesWS.svc?wsdl");
    connectionsManager->motionFileStoremanWSConnection()->setUrl("https://v21.pjwstk.edu.pl/HMDB/FileStoremanWS.svc?wsdl");

    //TODO
    //te 2 elementy są aktualnie niedostępne po stronie bazy!!
    //connectionsManager->medicalBasicQueriesWSConnection()->setUrl("https://v21.pjwstk.edu.pl/MotionMed/res/BasicQueriesWSStandalone.wsdl");
    //connectionsManager->medicalBasicUpdatesWSConnection()->setUrl("https://v21.pjwstk.edu.pl/MotionMed/BasicUpdatesWS.svc?wsdl");
    connectionsManager->medicalFileStoremanWSConnection()->setUrl("https://v21.pjwstk.edu.pl/HMDBMed/FileStoremanWS.svc?wsdl");


	//inicalizuję połączenie ftp - tutaj nic się nie dzieje, żadne połączenie z internetem nie jest nawiązywane
    connectionsManager->motionFtps()->setUrl("ftps://v21.pjwstk.edu.pl/");
    connectionsManager->motionFtps()->setCredentials("testUser", "testUser");
    connectionsManager->medicalFtps()->setUrl("ftps://v21.pjwstk.edu.pl/");
    connectionsManager->medicalFtps()->setCredentials("testUser", "testUser");

    //konfigurujemy web serwisy
    auto servicesManager = DataSourceWebServicesManager::create();

	//konfiguracja defaultowa dla usługi tworzącej userów w bazie - systemowy użytkownik
	connectionsManager->accountFactoryWSConnection()->setCredentials("hmdbServiceUser", "4accountCreation");
	//ustawiamy połączenia serwisom, ale połączenia jeszcze nie są gotowe - leniwa inicjalizacja
	servicesManager->accountFactoryService()->setConnection(connectionsManager->accountFactoryWSConnection());
    servicesManager->administrationService()->setConnection(connectionsManager->administrationWSConnection());
    servicesManager->authorizationService()->setConnection(connectionsManager->authorizationWSConnection());
    servicesManager->userPersonalSpaceService()->setConnection(connectionsManager->userPersonalSpaceWSConnection());

    servicesManager->motionBasicQueriesService()->setConnection(connectionsManager->motionBasicQueriesWSConnection());
    servicesManager->motionBasicUpdatesService()->setConnection(connectionsManager->motionBasicUpdatesWSConnection());
    servicesManager->motionFileStoremanService()->setConnection(connectionsManager->motionFileStoremanWSConnection());

    servicesManager->medicalBasicQueriesService()->setConnection(connectionsManager->medicalBasicQueriesWSConnection());
    servicesManager->medicalBasicUpdatesService()->setConnection(connectionsManager->medicalBasicUpdatesWSConnection());
    servicesManager->medicalFileStoremanService()->setConnection(connectionsManager->medicalFileStoremanWSConnection());

    //tworzymy i konfigurujemy instancje connectionManagera = odpowiada za sciąganie plików
    auto communication = CommunicationManager::getInstance();

	//TODO - obsługa pingu serwerów
	//adres serwara/serwerów do pingowania
    //communication->setUrlToPingServer(serwerPingUrl);


	//usługi do ściągania plików z danymi/metadanymi/zdjęciami
	communication->setMotionFileStoremanService(servicesManager->motionFileStoremanService());
    communication->setMedicalFileStoremanService(servicesManager->medicalFileStoremanService());

    communication->setMotionFtps(connectionsManager->motionFtps());
    communication->setMedicalFtps(connectionsManager->medicalFtps());

    localStorage = DataSourceLocalStorage::create();

    //inicjuję managera danych dla użytkowników ścieżką do wswólnej bazy danych
    localStorage->setLocalStorageDataPath(plugin::getPathInterface()->getUserApplicationDataPath() / "db" / "localStorage.db");

    //Zeruje aktualnego użytkownika
    currentUser_.setID(-1);

    //tworze instancje obiektu zarządzającego ścieżkami danych użytkowników
    pathsManager = DataSourcePathsManager::create();
    //inicjuję roota danych użytkowników podczas działania aplikacji (rozpakowywana z localStorage na bazie shallowCopy)
    pathsManager->setUsersDataPath(plugin::getPathInterface()->getTmpPath() / "data");
    pathsManager->setUser(currentUser_);
}

CommunicationDataSource::~CommunicationDataSource()
{
	//jeśli ktos zalogowany to czyscimy
	if(isLogged() == true){
		try{
			logout();
		}catch(std::exception & ){

		}
	}

	delete loginManager;
    CommunicationManager::destoryInstance();
    DataSourceWebServicesManager::destroy();
    DataSourceConnectionManager::destroy();
    DataSourcePathsManager::destroy();
    DataSourceLocalStorage::destroy();
}

void CommunicationDataSource::getOfferedTypes(utils::TypeInfoList & offeredTypes) const
{

}

void CommunicationDataSource::setConnectionsSerwerCertificatePath(const core::Filesystem::Path & certPath)
{
    auto connectionsManager = DataSourceConnectionManager::instance();
    std::string caPath = certPath.string();

	connectionsManager->accountFactoryWSConnection()->setCAPath(caPath);
    connectionsManager->administrationWSConnection()->setCAPath(caPath);
    connectionsManager->authorizationWSConnection()->setCAPath(caPath);
    connectionsManager->userPersonalSpaceWSConnection()->setCAPath(caPath);

    connectionsManager->motionBasicQueriesWSConnection()->setCAPath(caPath);
    connectionsManager->motionBasicUpdatesWSConnection()->setCAPath(caPath);
    connectionsManager->motionFileStoremanWSConnection()->setCAPath(caPath);

    connectionsManager->medicalBasicQueriesWSConnection()->setCAPath(caPath);
    connectionsManager->medicalBasicUpdatesWSConnection()->setCAPath(caPath);
    connectionsManager->medicalFileStoremanWSConnection()->setCAPath(caPath);
}

void CommunicationDataSource::setConnectionsCredentials(const User & user)
{
    auto connectionsManager = DataSourceConnectionManager::instance();

	//TODO weryfikować do jakich usług klient ma dostęp i tylko te mu konfigurować
	//connectionsManager->accountFactoryWSConnection()->setCredentials(user.name(), user.password());
    connectionsManager->administrationWSConnection()->setCredentials(user.name(), user.password());
    connectionsManager->authorizationWSConnection()->setCredentials(user.name(), user.password());
    connectionsManager->userPersonalSpaceWSConnection()->setCredentials(user.name(), user.password());

    connectionsManager->motionBasicQueriesWSConnection()->setCredentials(user.name(), user.password());
    connectionsManager->motionBasicUpdatesWSConnection()->setCredentials(user.name(), user.password());
    connectionsManager->motionFileStoremanWSConnection()->setCredentials(user.name(), user.password());

    connectionsManager->medicalBasicQueriesWSConnection()->setCredentials(user.name(), user.password());
    connectionsManager->medicalBasicUpdatesWSConnection()->setCredentials(user.name(), user.password());
    connectionsManager->medicalFileStoremanWSConnection()->setCredentials(user.name(), user.password());

    //connectionsManager->motionFtps()->setCredentials(user.name(), user.password());
    //connectionsManager->medicalFtps()->setCredentials(user.name(), user.password());
}


void CommunicationDataSource::init(core::IMemoryDataManager * memoryDM, core::IStreamDataManager * streamManager, core::IFileDataManager * fileDM)
{
    this->memoryDM = memoryDM;
    this->fileDM = fileDM;

    fileStatusManager.reset(new FileStatusManager(fileDM));
    fullShallowCopyStatus.reset(new DataSourceStatusManager(fileStatusManager.get()));

    UTILS_ASSERT(memoryDM != nullptr, "Niezainicjowany DM");
    UTILS_ASSERT(fileDM != nullptr, "Niezainicjowany DM");
}

QWidget* CommunicationDataSource::getWidget()
{
    if(dataSourceWidget == nullptr){
        dataSourceWidget = new DataSourceWidget(this);
    }

    return dataSourceWidget;
}

void CommunicationDataSource::setCurrentUser(const User & user)
{
    currentUser_ = user;
    //aktualizuje połaczenia z webserwices
    setConnectionsCredentials(currentUser_);
    //inicjuje pathManagera - tworzę strukturę danych użytkownika i generuje ścieżki dla danych
    pathsManager->setUser(currentUser_);
}

void CommunicationDataSource::login(const std::string & user, const std::string & password)
{
    if(isLogged() == true){
        throw std::runtime_error("Already logged in");
    }

    bool connError = false;

	if(offlineMode_ == false){

		try{
			loginManager->login(user, password);
		}catch(std::exception & e){
			connError = true;
			PLUGIN_LOG_ERROR("Error during login: " << e.what());
		}catch(...){
			connError = true;
			PLUGIN_LOG_ERROR("Unknown error during login");
		}

	}

    if(loginManager->isLogged() == true){
        setCurrentUser(loginManager->user());
    }else if(offlineMode_ == true || connError == true){
        //próbujemy lokalnie zalogować
        //sprawdzam czy użytkownik ma kompletną płytką kopię bazy danych - jeśli tak to go loguję offline
        User tmpUser;
        tmpUser.setName(user);
        tmpUser.setPassword(password);
        tmpUser.setID(-2);

        OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*pathsManager);

        pathsManager->setUser(tmpUser);

        //jeśli mam pelna płytka kopie bazy danych
        if(isShallowCopyComplete() == true){
            setCurrentUser(tmpUser);
        }else{
            //nie mam płytkiej kopi bazy danych wiec resetuje
            pathsManager->setUser(currentUser_);
        }
    }else{
		//TODO
		//nie zalogowano i nie ma usera - może usunięty?
		//jeżli mamy takiego lokalnie to usuwamy z localStorage

	}

    //jeśli zalogowano poprawnie
    if(isLogged() == true){
        //tworzę ścieżki dla danych użytkownika
        pathsManager->createUserDataPaths();
        //informuj wszystkich o zalogowaniu
        notify();
    }
}

void CommunicationDataSource::logout()
{
    if(isLogged() == false){
        throw std::runtime_error("Already logout");
    }

    //czyszcze lokalne dane użytkownika rozpakowane z localStorage
    pathsManager->removeUserDataPaths();

    //resetuje użytkownika
    currentUser_ = User();
    currentUser_.setID(-1);
    //aktualizuje jego dane wszedzie gdzie to niezbędne
    pathsManager->setUser(currentUser_);

    //wyczyśc płytką kopię bazy danych
    fullShallowCopy = ShallowCopy();
	fileStatusManager->removeAllFiles();
	fullShallowCopyStatus->setShallowCopy(nullptr);

    //notyfikuj o zmianie stanu
    notify();
}

bool CommunicationDataSource::isLogged() const
{
    return currentUser_.id() != -1;
}

void CommunicationDataSource::setOfflineMode(bool offline)
{
	offlineMode_ = offline;
}

bool CommunicationDataSource::offlineMode() const
{
	return offlineMode_;
}

const User * CommunicationDataSource::currentUser() const
{
    return &currentUser_;
}

void CommunicationDataSource::showPatientCard(const PluginSubject::SubjectConstPtr & subject, const communication::PatientConstPtr & patient)
{
	webservices::MotionShallowCopy::Performer * sub = nullptr;
	webservices::MedicalShallowCopy::Patient * pat = nullptr;

	if(subject != nullptr){
		//wyszukuję subjecta
		for(auto subIT = dataSourceWidget->subjectsMapping.begin(); subIT != dataSourceWidget->subjectsMapping.end(); ++subIT){
			PluginSubject::SubjectConstPtr tmpSub = subIT->second.first->get();
			if(tmpSub != nullptr && tmpSub->getID() == subject->getID()){
				sub = fullShallowCopy.motionShallowCopy->performers.find(subIT->first)->second;
				break;
			}
		}
	}

	if(patient != nullptr){
		//wyszukujemy patienta
		for(auto pIT = dataSourceWidget->patientsMapping.begin(); pIT != dataSourceWidget->patientsMapping.end(); ++pIT){
			communication::PatientConstPtr tmpPat = pIT->second.first->get();
			if(tmpPat != nullptr && tmpPat->getID() == subject->getID()){
				pat = fullShallowCopy.medicalShallowCopy->patients.find(pIT->first)->second;
				break;
			}
		}
	}

	QMetaObject::invokeMethod(dataSourceWidget, "setPatientCard", Q_ARG(webservices::MedicalShallowCopy::Patient *, pat),
		Q_ARG(webservices::MotionShallowCopy::Performer *, sub));

	QMetaObject::invokeMethod(dataSourceWidget, "showPatientCard");
}

void CommunicationDataSource::showUserDataCard()
{
	QMetaObject::invokeMethod(dataSourceWidget, "showUserData");
}

void CommunicationDataSource::showConfigurationCard()
{
	QMetaObject::invokeMethod(dataSourceWidget, "showConfiguration");
}

bool CommunicationDataSource::isShallowCopyComplete() const
{
    bool ret = false;
    {
        OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*pathsManager);
        try{
            if(localStorage->fileIsLocal(pathsManager->motionShallowCopyPath().filename().string()) &&
                localStorage->fileIsLocal(pathsManager->motionMetadataPath().filename().string()) &&
                localStorage->fileIsLocal(pathsManager->medicalShallowCopyPath().filename().string()) &&
                localStorage->fileIsLocal(pathsManager->medicalMetadataPath().filename().string())){

                ret = true;
            }
        }catch(...){

        }
    }
    return ret;
}

bool CommunicationDataSource::isShallowCopyCurrent()
{
    OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*DataSourceWebServicesManager::instance()->motionBasicQueriesService());
    auto lastMod = DataSourceWebServicesManager::instance()->motionBasicQueriesService()->dataModificationTime();

    return DataSourceShallowCopyUtils::shallowCopyRequiresRefresh(fullShallowCopy, lastMod);
}

CommunicationDataSource::DownloadRequestPtr CommunicationDataSource::generateDownloadShallowCopyRequestToLocalUserSpace()
{
    std::map<CommunicationManager::Request, DownloadRequest::ShallowCopyDescriptor> reqData;

    DownloadRequest::ShallowCopyDescriptor desc;
    desc.destPath = pathsManager->motionShallowCopyPath().string();
    desc.size = -1;

    reqData.insert(std::map<CommunicationManager::Request, DownloadRequest::ShallowCopyDescriptor>::value_type(CommunicationManager::CopyMotionShallowCopy, desc));

    desc.destPath = pathsManager->motionMetadataPath().string();

    reqData.insert(std::map<CommunicationManager::Request, DownloadRequest::ShallowCopyDescriptor>::value_type(CommunicationManager::CopyMotionMetadata, desc));

    desc.destPath = pathsManager->medicalShallowCopyPath().string();

    reqData.insert(std::map<CommunicationManager::Request, DownloadRequest::ShallowCopyDescriptor>::value_type(CommunicationManager::CopyMedicalShallowCopy, desc));

    desc.destPath = pathsManager->medicalMetadataPath().string();

    reqData.insert(std::map<CommunicationManager::Request, DownloadRequest::ShallowCopyDescriptor>::value_type(CommunicationManager::CopyMedicalMetadata, desc));

    return CommunicationDataSource::DownloadRequestPtr(DownloadRequest::createShallowCopyRequest(reqData));
}

void CommunicationDataSource::extractShallowCopyFromLocalStorageToUserSpace()
{
    std::vector<core::Filesystem::Path> extractedFiles;

    try{
        //płytka kopia danych ruchu
        auto path = pathsManager->motionShallowCopyPath();
		auto filename = path.filename().string();

		extractFileFromLocalStorage(filename, path, extractedFiles);

        //płytka kopia metadanych ruchu
        path = pathsManager->motionMetadataPath();
		filename = path.filename().string();
        extractFileFromLocalStorage(filename, path, extractedFiles);

        //płytka kopia danych medycznych
        path = pathsManager->medicalShallowCopyPath();
		filename = path.filename().string();
        extractFileFromLocalStorage(filename, path, extractedFiles);

        //płytka kopia metadanych medycznych
        path = pathsManager->medicalMetadataPath();
		filename = path.filename().string();
        extractFileFromLocalStorage(filename, path, extractedFiles);
    }catch(...){
        //próbujemy czyścić

        for(auto it = extractedFiles.begin(); it != extractedFiles.end(); ++it){
            try{
                core::Filesystem::deleteFile(*it);
            }catch(...){

            }
        }

        throw;
    }
}

bool CommunicationDataSource::copyData(QIODevice &inFile, QIODevice &outFile)
{
	while (!inFile.atEnd()) {
		char buf[4096];
		qint64 readLen = inFile.read(buf, 4096);
		if (readLen <= 0)
			return false;
		if (outFile.write(buf, readLen) != readLen)
			return false;
	}
	return true;
}

void CommunicationDataSource::extractDataFromLocalStorageToUserSpace()
{
    std::vector<core::Filesystem::Path> extractedFiles;

    auto sessionsItEnd = fullShallowCopy.motionShallowCopy->sessions.end();
    for(auto sessionIT = fullShallowCopy.motionShallowCopy->sessions.begin(); sessionIT != sessionsItEnd; ++sessionIT){
        try{
            core::Filesystem::createDirectory(pathsManager->sessionPath(sessionIT->second->sessionName));
            auto filesItEnd = sessionIT->second->files.end();
            for(auto fileIT = sessionIT->second->files.begin(); fileIT != filesItEnd; ++fileIT){
                extractFileFromLocalStorageToUserSpace(fileIT->second, sessionIT->second->sessionName);
            }

            auto motionsItEnd = sessionIT->second->trials.end();
            for(auto motionIT = sessionIT->second->trials.begin(); motionIT != motionsItEnd; ++motionIT){

                auto filesItEnd = motionIT->second->files.end();
                for(auto fileIT = motionIT->second->files.begin(); fileIT != filesItEnd; ++fileIT){
                    extractFileFromLocalStorageToUserSpace(fileIT->second, sessionIT->second->sessionName);
                }
            }
        }catch(...){

        }
    }
}

void CommunicationDataSource::extractFileFromLocalStorageToUserSpace(const webservices::MotionShallowCopy::File * file, const std::string & sessionName)
{
    auto filePath = pathsManager->filePath(file->fileName, sessionName);

    if(fileStatusManager->fileExists(file->fileID) == true){
        fileStatusManager->setFilePath(file->fileID, filePath);
    }else{
        fileStatusManager->addFile(file->fileID, filePath);
    }

    if(localStorage->fileIsLocal(file->fileName) == true){
        try{
            //czy zadana ściezka istnieje?
            core::Filesystem::Path p(filePath.parent_path());
            if(core::Filesystem::pathExists(p) == false){
                core::Filesystem::createDirectory(p);
            }

            localStorage->extractFile(file->fileName, filePath);
            fileStatusManager->setFileStorage(file->fileID, Local);
        }catch(...){
            fileStatusManager->setFileStorage(file->fileID, Remote);
        }
    }else{
        fileStatusManager->setFileStorage(file->fileID, Remote);
    }

	//próbuje pliki vsk wypakować z zip

	auto zipName = sessionName + ".zip";
	auto vskName = sessionName + ".vsk";

	if(file->fileName == zipName){
		//mam zipa - szukam vsk w zip
		auto vskDestPath = pathsManager->filePath(vskName, sessionName);
		QuaZip zip(filePath.string().c_str());

		if(zip.open(QuaZip::mdUnzip) == true){
			//udało sie otworzyć plik do wypakowywania
			auto files = zip.getFileNameList();
			if(files.contains(QString::fromUtf8(vskName.c_str())) == true &&
				zip.setCurrentFile(QString::fromUtf8(vskName.c_str())) == true){

				//znalazłem vsk - próbujemy go wypakować
				QuaZipFile inFile(&zip);
				if(inFile.open(QIODevice::ReadOnly) && inFile.getZipError() == UNZ_OK) {

					//tworze plik docelowy
					QFile outFile;
					outFile.setFileName(vskDestPath.string().c_str());
					if(outFile.open(QIODevice::WriteOnly)) {

						//udało się - mogę kopiować
						auto ok = copyData(inFile, outFile);
						ok &= (inFile.getZipError()==UNZ_OK);

						//zamykam plik docelowy
						outFile.close();

						//zamykam plik w archiwum zip
						inFile.close();

						if ( ok == false || inFile.getZipError()!=UNZ_OK) {
							core::Filesystem::deleteFile(vskDestPath);
						}
					}
				}
			}
		}
	}
}

void CommunicationDataSource::extractDataFromLocalStorageToUserSpace(const ShallowCopy & prevShallowCopy, const ShallowCopy & newShallowCopy)
{
    std::set<int> prevFiles;
    std::set<int> newFiles;

    std::vector<int> toDelete;
    std::vector<int> toAdd;
    std::vector<int> toRefresh;

    //poprzednie pliki
    if(prevShallowCopy.motionShallowCopy != nullptr){
        auto filesITEnd = prevShallowCopy.motionShallowCopy->files.end();
        for(auto fileIT = prevShallowCopy.motionShallowCopy->files.begin(); fileIT != filesITEnd; ++fileIT){
            prevFiles.insert(fileIT->first);
        }
    }

    //aktualne pliki
    auto filesITEnd = newShallowCopy.motionShallowCopy->files.end();
    for(auto fileIT = newShallowCopy.motionShallowCopy->files.begin(); fileIT != filesITEnd; ++fileIT){
        newFiles.insert(fileIT->first);
    }

    //zapewniam minimalny rozmiar wektorów wynikowych
    //do usunięcia
    toDelete.resize(prevFiles.size());
    //do dodania
    toAdd.resize(newFiles.size());
    //do odówieżenia
    toRefresh.resize((std::min)(prevFiles.size(), newFiles.size()));

    //wyznaczam różnice zbiorów
    auto toDeleteIT = std::set_difference(prevFiles.begin(), prevFiles.end(), newFiles.begin(), newFiles.end(), toDelete.begin());
    auto toAddIT = std::set_difference(newFiles.begin(), newFiles.end(), prevFiles.begin(), prevFiles.end(), toAdd.begin());

    //część wspólna do odówieżenia
    auto toRefreshIT = std::set_intersection(newFiles.begin(), newFiles.end(), prevFiles.begin(), prevFiles.end(), toRefresh.begin());

    //część właściwa
    //usuwamy pliki których nie powinno być
    for(auto it = toDelete.begin(); it != toDeleteIT; ++it){
        //auto file = prevShallowCopy.motionShallowCopy->files.find(*it)->second;
        try{
			core::Filesystem::deleteFile(fileStatusManager->filePath(*it));
        }catch(...){

        }

		try{
			fileStatusManager->removeFile(*it);
		}catch(...){

		}
    }

    //dodajemy pliki do kótrych mamy dostęp, są lokalne a jeszcze ich nie ma
    for(auto it = toAdd.begin(); it != toAddIT; ++it){
        auto file = newShallowCopy.motionShallowCopy->files.find(*it)->second;
        extractFileFromLocalStorageToUserSpace(file, file->isSessionFile() ? file->session->sessionName : file->trial->session->sessionName);
    }

    //odsiwezamy pliki które teoretycznie powinny być
    for(auto it = toRefresh.begin(); it != toRefreshIT; ++it){
        auto file = prevShallowCopy.motionShallowCopy->files.find(*it)->second;
        auto filePath = pathsManager->filePath(file->fileName, file->isSessionFile() == true ? file->session->sessionName : file->trial->session->sessionName);
        try{
            if(core::Filesystem::pathExists(filePath) == false && localStorage->fileIsLocal(file->fileName) == true){
                extractFileFromLocalStorageToUserSpace(file, file->isSessionFile() ? file->session->sessionName : file->trial->session->sessionName);
            }
        }catch(...){

        }
    }
}

void CommunicationDataSource::extractFileFromLocalStorage(const std::string & fileName, const core::Filesystem::Path & destPath, std::vector<core::Filesystem::Path> & extractedFilesPaths)
{
    localStorage->extractFile(fileName, destPath);
    extractedFilesPaths.push_back(destPath);
}

bool CommunicationDataSource::buildShallowCopyFromLocalUserSpace(communication::ShallowCopy & shallowCopy)
{
    bool ret = false;

    ShallowCopy tmpShallow;

    DataSourceShallowCopyUtils::createShallowCopy(tmpShallow, pathsManager->motionShallowCopyPath(),
        pathsManager->motionMetadataPath(), pathsManager->medicalShallowCopyPath(), pathsManager->medicalMetadataPath());

    ret = DataSourceShallowCopyUtils::checkShallowCopyIntegrity(tmpShallow);

    std::swap(shallowCopy, tmpShallow);

    return ret;
}

void CommunicationDataSource::saveShallowCopyInLocalStorage()
{
    try{
        localStorage->loadFile(pathsManager->motionShallowCopyPath(), pathsManager->motionShallowCopyPath().filename().string());
    }catch(...){

    }

    try{
        localStorage->loadFile(pathsManager->motionMetadataPath(), pathsManager->motionMetadataPath().filename().string());
    }catch(...){

    }

    try{
        localStorage->loadFile(pathsManager->medicalShallowCopyPath(), pathsManager->medicalShallowCopyPath().filename().string());
    }catch(...){

    }

    try{
        localStorage->loadFile(pathsManager->medicalMetadataPath(), pathsManager->medicalMetadataPath().filename().string());
    }catch(...){

    }
}

void CommunicationDataSource::removeShallowCopyFromUserSpace()
{
    //płytka kopia danych ruchu
    try{
        core::Filesystem::deleteFile(pathsManager->motionShallowCopyPath());
    }catch(...){

    }

    //płytka kopia metadanych ruchu
    try{
        core::Filesystem::deleteFile(pathsManager->motionMetadataPath());
    }catch(...){

    }

    //płytka kopia danych medycznych
    try{
        core::Filesystem::deleteFile(pathsManager->medicalShallowCopyPath());
    }catch(...){

    }

    //płytka kopia metadanych medycznych
    try{
        core::Filesystem::deleteFile(pathsManager->medicalMetadataPath());
    }catch(...){

    }
}

void CommunicationDataSource::removeShallowCopyFromLocalStorage()
{
    OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*pathsManager);

    //płytka kopia danych ruchu
    try{
        localStorage->removeFile(pathsManager->motionShallowCopyPath().filename().string());
    }catch(...){

    }

    //płytka kopia metadanych ruchu
    try{
        localStorage->removeFile(pathsManager->motionMetadataPath().filename().string());
    }catch(...){

    }

    //płytka kopia danych medycznych
    try{
        localStorage->removeFile(pathsManager->medicalShallowCopyPath().filename().string());
    }catch(...){

    }

    //płytka kopia metadanych medycznych
    try{
        localStorage->removeFile(pathsManager->medicalMetadataPath().filename().string());
    }catch(...){

    }
}

void CommunicationDataSource::setShallowCopy(const ShallowCopy & shallowCopy)
{
    //różnica danych wypakowanych z localStorage + status plików jeśli chodzi o storage
    //extractDataFromLocalStorageToUserSpace(fullShallowCopy, shallowCopy);

    fullShallowCopy = shallowCopy;

	refreshFileManager();

    //aktualizuj status danych
    fullShallowCopyStatus->setShallowCopy(&fullShallowCopy);
}

bool CommunicationDataSource::tryActivateAccount(const std::string & login, const std::string & activationCode)
{
	bool ret = false;
	try{
		ret = DataSourceWebServicesManager::instance()->accountFactoryService()->activateUserAccount(login, activationCode, true);
	}catch(std::exception & ){

	}catch(...){

	}

	return ret;
}

bool CommunicationDataSource::tryResetPassword(const std::string & email)
{
	bool ret = false;
	try{
		ret = DataSourceWebServicesManager::instance()->accountFactoryService()->resetPassword(email, true);
	}catch(std::exception & ){

	}catch(...){

	}

	return ret;
}

bool CommunicationDataSource::registerUser(const std::string & login, const std::string & email, const std::string & password,
	const std::string & firstName, const std::string & lastName)
{
	bool ret = false;
	try{
		DataSourceWebServicesManager::instance()->accountFactoryService()->createUserAccount(login, email, password, firstName, lastName, true);
		ret = true;
	}catch(std::exception & ){

	}catch(...){

	}

	return ret;
}

void CommunicationDataSource::refreshFileManager()
{
    auto itEnd = fullShallowCopy.motionShallowCopy->files.end();
    for(auto it = fullShallowCopy.motionShallowCopy->files.begin(); it != itEnd; ++it){
        auto file = it->second;
        auto filePath = pathsManager->filePath(file->fileName, file->isSessionFile() == true ? file->session->sessionName : file->trial->session->sessionName);

        if(fileStatusManager->fileExists(file->fileID) == false){

			communication::DataStorage storage = communication::Remote;

			if(localStorage->fileIsLocal(file->fileName) == true){
				storage = communication::Local;
			}

            fileStatusManager->addFile(file->fileID, filePath, DataStatus(storage, communication::Unloaded));
        }else{
            fileStatusManager->setFilePath(file->fileID, filePath);
        }
	}
}

CommunicationDataSource::DownloadRequestPtr CommunicationDataSource::generateDownloadFileRequest(int fileID)
{
    std::set<int> files;
    files.insert(fileID);
    return generateDownloadFilesRequest(files);
}

CommunicationDataSource::DownloadRequestPtr CommunicationDataSource::generateDownloadFilesRequest(const std::set<int> & fileIDs)
{
    std::map<int, DownloadRequest::FileDescriptor> files;

    for(auto it = fileIDs.begin(); it != fileIDs.end(); ++it){
        auto fileIT = fullShallowCopy.motionShallowCopy->files.find(*it);
        auto file = fileIT->second;

        DownloadRequest::FileDescriptor desc;
        desc.size = file->fileSize;
        desc.id = *it;

        auto sessionName = file->isSessionFile() == true ? file->session->sessionName : file->trial->session->sessionName;

        desc.destPath = pathsManager->filePath(file->fileName, sessionName).string();

        //musimy zagwarantować że mamy gdzie zpaisywać więc ścieżki do sesji są tworzone

        auto sessionPath = pathsManager->sessionPath(sessionName);

        if(core::Filesystem::pathExists(sessionPath) == false){
            core::Filesystem::createDirectory(sessionPath);
        }

        files.insert(std::map<int, DownloadRequest::FileDescriptor>::value_type(desc.id, desc));
    }

    return DownloadRequestPtr(DownloadRequest::createFilesRequest(files));
}
