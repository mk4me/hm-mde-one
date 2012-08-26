#include <webserviceslib/AuthorizationWS.h>
#include <utils/Debug.h>
#include <tinyxml.h>

namespace webservices
{

AuthorizationWS::AuthorizationWS(const WSConnectionPtr & connection) : connection_(connection), constConnection_(connection)
{
	mutex = this;	
}

AuthorizationWS::~AuthorizationWS()
{

}

void AuthorizationWS::setConnection(const WSConnectionPtr & connection)
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	connection_ = connection;
	constConnection_ = connection;
}

const WSConnectionPtr & AuthorizationWS::connection()
{
	return connection_;
}

const WSConnectionConstPtr & AuthorizationWS::connection() const
{
	return constConnection_;
}

const bool AuthorizationWS::checkMyLogin() const
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	webservices::IWSConnection::ScopedLock lock_(*connection_);
	connection_->setOperation("CheckMyLogin");

	try{
		connection_->invoke(true);
	}catch(WSConnectionSecurityException & ){
		//nie mam usera
		return false;
	}catch(...){
		//problem inny niż prawa do usług
		//rzucam dalej
		throw;
	}

	bool ret = false;

	try{
		connection_->getValue<bool>("CheckMyLoginResult", ret);
	}catch(...){

	}

	return ret;
}

void AuthorizationWS::createUserAccount(const std::string & name, const std::string & surname)
{

}

void AuthorizationWS::listUsers() const
{

}

void AuthorizationWS::listSessionPrivileges(int sessionID) const
{

}

void AuthorizationWS::grantSessionPrivileges(const std::string & grantedUserLogin, int sessionID, bool write)
{

}

void AuthorizationWS::removeSessionPrivileges(const std::string & grantedUserLogin, int sessionID)
{

}

void AuthorizationWS::alterSessionVisibility(int sessionID, bool isPublic, bool isWritable)
{

}

}
