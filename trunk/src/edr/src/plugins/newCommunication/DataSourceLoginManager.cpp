#include "CommunicationPCH.h"
#include "DataSourceLoginManager.h"
#include "DataSourceWebServicesManager.h"

DataSourceLoginManager::DataSourceLoginManager()
{
    user_.setID(-1);
}

DataSourceLoginManager::~DataSourceLoginManager()
{

}

void DataSourceLoginManager::login(const std::string & user, const std::string & password)
{
	int userID = getUserIDForLoginAndPassword(user, password);
    
    if(userID == -1){
        return;
    }

    UserPrivilages privilages;
    getUserPrivilages(userID, privilages);

    UserData data;
    getUserData(userID, data);

    user_.setID(userID);
    user_.setPrivilages(privilages);
    user_.setData(data);
	user_.setName(user);
	user_.setPassword(password);
}

void DataSourceLoginManager::logout()
{
    //TODO
    //zapis zmienionych danych u�ytkownika w bazie!!
    //w przypadku niepowodzenia serializacja do pliku!!
    //potem takie dane powinny by� wczytywane z dysku i ponownie musi nastapi� pr�ba zapisu

    user_.setID(-1);
    user_.setPrivilages(UserPrivilages());
    user_.setData(UserData());
}

const User & DataSourceLoginManager::user() const
{
    return user_;
}

const bool DataSourceLoginManager::isLogged() const
{
    return user_.id() != -1;
}

void DataSourceLoginManager::getUserData(int userID, UserData & userData)
{
    //TODO
    //doda� �ci�ganie i parsowanie danych u�ytkownika z bazy
}

void DataSourceLoginManager::getUserPrivilages(int userID, UserPrivilages & userPrivilages)
{
    //TODO
    //doda� �ci�ganie i parsowanie uprawnie� do danych u�ytkownika
}

const int DataSourceLoginManager::getUserIDForLoginAndPassword(const std::string & login, const std::string & password)
{
    OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*DataSourceWebServicesManager::instance()->authorizationService());
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lockConn(*DataSourceWebServicesManager::instance()->authorizationService()->connection());

	int ret = -1;

	auto lUser = DataSourceWebServicesManager::instance()->authorizationService()->connection()->user();
	auto lPassword = DataSourceWebServicesManager::instance()->authorizationService()->connection()->password();

	DataSourceWebServicesManager::instance()->authorizationService()->connection()->setCredentials(login, password);
	bool res = DataSourceWebServicesManager::instance()->authorizationService()->checkMyLogin();

	if(res == true){
		//TODO
		//czym identyfikujemy u�ytkownika?
		ret = 1;

	}

	DataSourceWebServicesManager::instance()->authorizationService()->connection()->setCredentials(lUser, lPassword);

	return ret;
}