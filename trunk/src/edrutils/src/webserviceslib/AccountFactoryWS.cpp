#include <webserviceslib/AccountFactoryWS.h>

namespace webservices {

AccountFactoryWS::AccountFactoryWS(const WSConnectionPtr & connection) : connection_(connection), constConnection_(connection)
{
	mutex = this;
}

AccountFactoryWS::~AccountFactoryWS()
{

}

void AccountFactoryWS::setConnection(const WSConnectionPtr & connection)
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	connection_ = connection;
	constConnection_ = connection;
}

const WSConnectionPtr & AccountFactoryWS::connection()
{
	return connection_;
}

const WSConnectionConstPtr & AccountFactoryWS::connection() const
{
	return constConnection_;
}

void AccountFactoryWS::createUserAccount(const std::string & login, const std::string & email, const std::string & password,
	const std::string & firstName, const std::string & lastName)
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	webservices::IWSConnection::ScopedLock lockConn(*connection_);
	connection_->setOperation("CreateUserAccount");
	connection_->setValue("login", login);
	connection_->setValue("email", email);
	connection_->setValue("pass", password);
	connection_->setValue("firstName", firstName);
	connection_->setValue("lastName", lastName);
	connection_->invoke();
}

bool AccountFactoryWS::activateUserAccount(const std::string & login, const std::string & activationCode)
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	webservices::IWSConnection::ScopedLock lockConn(*connection_);
	connection_->setOperation("ActivateUserAccount");
	connection_->setValue("login", login);
	connection_->setValue("activationCode", activationCode);
	connection_->invoke(true);
	bool ret = false;
	connection_->getValue("ActivateUserAccountResult", ret);

	return ret;
}

}