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
	int userID = getUserID();
    
    if(userID == -1){
        return;
    }

    UserPrivilages privilages;
    getUserPrivilages(userID, privilages);

    UserData data;
    getUserData(userID, data);

	communication::IUser::UserGroups userGroupsList;
	getUserGroups(userGroupsList);

    user_.setID(userID);
    user_.setUserPrivilages(privilages);
    user_.setUserData(data);
	user_.setUserGroups(userGroupsList);
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
    user_.setUserPrivilages(UserPrivilages());
    user_.setUserData(UserData());
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

void DataSourceLoginManager::getUserGroups(communication::IUser::UserGroups & userGroups)
{
	try{
		auto response = DataSourceWebServicesManager::instance()->authorizationService()->listMyUserGroupsAssigned();
		auto wuserGroups = webservices::xmlWsdl::parseUserGroups(response);

		for(auto it = wuserGroups.begin(); it != wuserGroups.end(); ++it){
			userGroups[(*it).id] = (*it).name;
		}

	}catch(...){

	}
}

const int DataSourceLoginManager::getUserID()
{
	int ret = -1;
	bool res = DataSourceWebServicesManager::instance()->authorizationService()->checkMyLogin();

	if(res == true){
		
		auto xml = DataSourceWebServicesManager::instance()->authorizationService()->getMyUserData();
		ret = webservices::xmlWsdl::parseUser(xml).id;

	}

	return ret;
}