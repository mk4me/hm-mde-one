#include <webserviceslib/AccountFactoryWS.h>

namespace webservices {

SingleAccountFactoryWS::SingleAccountFactoryWS(const WSConnectionPtr & connection) : connection_(connection), constConnection_(connection)
{
	mutex = this;
}

SingleAccountFactoryWS::~SingleAccountFactoryWS()
{

}

void SingleAccountFactoryWS::setConnection(const WSConnectionPtr & connection)
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	connection_ = connection;
	constConnection_ = connection;
}

const WSConnectionPtr & SingleAccountFactoryWS::connection()
{
	return connection_;
}

const WSConnectionConstPtr & SingleAccountFactoryWS::connection() const
{
	return constConnection_;
}

void SingleAccountFactoryWS::createUserAccount(const std::string & login, const std::string & email, const std::string & password,
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

bool SingleAccountFactoryWS::activateUserAccount(const std::string & login, const std::string & activationCode)
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

//Multi

MultiAccountFactoryWS::MultiAccountFactoryWS(const WSConnectionPtr & connection) : connection_(connection), constConnection_(connection)
{
	mutex = this;
}

MultiAccountFactoryWS::~MultiAccountFactoryWS()
{

}

void MultiAccountFactoryWS::setConnection(const WSConnectionPtr & connection)
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	connection_ = connection;
	constConnection_ = connection;
}

const WSConnectionPtr & MultiAccountFactoryWS::connection()
{
	return connection_;
}

const WSConnectionConstPtr & MultiAccountFactoryWS::connection() const
{
	return constConnection_;
}

void MultiAccountFactoryWS::createUserAccount(const std::string & login, const std::string & email, const std::string & password,
	const std::string & firstName, const std::string & lastName, bool propagateToHMDB)
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	webservices::IWSConnection::ScopedLock lockConn(*connection_);
	connection_->setOperation("CreateUserAccount");
	connection_->setValue("login", login);
	connection_->setValue("email", email);
	connection_->setValue("pass", password);
	connection_->setValue("firstName", firstName);
	connection_->setValue("lastName", lastName);
	connection_->setValue("propagateToHMDB", propagateToHMDB);
	connection_->invoke();
}

bool MultiAccountFactoryWS::activateUserAccount(const std::string & login, const std::string & activationCode, bool propagateToHMDB)
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	webservices::IWSConnection::ScopedLock lockConn(*connection_);
	connection_->setOperation("ActivateUserAccount");
	connection_->setValue("login", login);
	connection_->setValue("activationCode", activationCode);
	connection_->setValue("propagateToHMDB", propagateToHMDB);
	connection_->invoke(true);
	bool ret = false;
	connection_->getValue("ActivateUserAccountResult", ret);

	return ret;
}

}