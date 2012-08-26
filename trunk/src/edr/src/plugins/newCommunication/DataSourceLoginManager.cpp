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
    //zapis zmienionych danych użytkownika w bazie!!
    //w przypadku niepowodzenia serializacja do pliku!!
    //potem takie dane powinny być wczytywane z dysku i ponownie musi nastapić próba zapisu

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
    //dodać ściąganie i parsowanie danych użytkownika z bazy
}

void DataSourceLoginManager::getUserPrivilages(int userID, UserPrivilages & userPrivilages)
{
    //TODO
    //dodać ściąganie i parsowanie uprawnień do danych użytkownika
}

const int DataSourceLoginManager::getUserIDForLoginAndPassword(const std::string & login, const std::string & password)
{
    OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*DataSourceWebServicesManager::instance()->authorizationService());
	auto conn = DataSourceWebServicesManager::instance()->authorizationService()->connection();
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lockConn(*conn);

	int ret = -1;

	auto lUser = conn->user();
	auto lPassword = conn->password();

	conn->setCredentials(login, password);
	bool res = DataSourceWebServicesManager::instance()->authorizationService()->checkMyLogin();

	if(res == true){
		//TODO
		//czym identyfikujemy użytkownika?
		ret = 1;

	}

	conn->setCredentials(lUser, lPassword);

	return ret;
}
