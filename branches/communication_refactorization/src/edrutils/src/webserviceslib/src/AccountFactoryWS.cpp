#include <webserviceslib/AccountFactoryWS.h>

namespace webservices {

SingleAccountFactoryWS::SingleAccountFactoryWS(const WSConnectionPtr & connection)
	: WebServiceT<ISingleAccountFactoryWS>(connection)
{

}

SingleAccountFactoryWS::~SingleAccountFactoryWS()
{

}

void SingleAccountFactoryWS::createUserAccount(const std::string & login,
	const std::string & email, const std::string & password,
	const std::string & firstName, const std::string & lastName)
{
	connection()->setOperation("CreateUserAccount");
	connection()->setValue("login", login);
	connection()->setValue("email", email);
	connection()->setValue("pass", password);
	connection()->setValue("firstName", firstName);
	connection()->setValue("lastName", lastName);
	connection()->invoke();
}

const bool SingleAccountFactoryWS::activateUserAccount(const std::string & login,
	const std::string & activationCode)
{
	connection()->setOperation("ActivateUserAccount");
	connection()->setValue("login", login);
	connection()->setValue("activationCode", activationCode);
	connection()->invoke(true);
	
	bool ret = false;
	connection()->getValue("ActivateUserAccountResult", ret);

	return ret;
}

const bool SingleAccountFactoryWS::resetPassword(const std::string & email)
{
	connection()->setOperation("ResetPassword");
	connection()->setValue("email", email);
	connection()->invoke(true);
	
	bool ret = false;
	connection()->getValue("ResetPasswordResult", ret);

	return ret;
}

//Multi

MultiAccountFactoryWS::MultiAccountFactoryWS(const WSConnectionPtr & connection)
	: WebServiceT<IMultiAccountFactoryWS>(connection)
{

}

MultiAccountFactoryWS::~MultiAccountFactoryWS()
{

}

void MultiAccountFactoryWS::createUserAccount(const std::string & login,
	const std::string & email, const std::string & password,
	const std::string & firstName, const std::string & lastName,
	const bool propagateToHMDB)
{
	connection()->setOperation("CreateUserAccount");
	connection()->setValue("login", login);
	connection()->setValue("email", email);
	connection()->setValue("pass", password);
	connection()->setValue("firstName", firstName);
	connection()->setValue("lastName", lastName);
	connection()->setValue("propagateToHMDB", propagateToHMDB);
	connection()->invoke();
}

const bool MultiAccountFactoryWS::activateUserAccount(const std::string & login,
	const std::string & activationCode, const bool propagateToHMDB)
{
	connection()->setOperation("ActivateUserAccount");
	connection()->setValue("login", login);
	connection()->setValue("activationCode", activationCode);
	connection()->setValue("propagateToHMDB", propagateToHMDB);
	connection()->invoke(true);
	
	bool ret = false;
	connection()->getValue("ActivateUserAccountResult", ret);

	return ret;
}

const bool MultiAccountFactoryWS::resetPassword(const std::string & email,
	const bool propagateToHMDB)
{
	connection()->setOperation("ResetPassword");
	connection()->setValue("propagateToHMDB", propagateToHMDB);
	connection()->setValue("email", email);
	connection()->invoke(true);

	bool ret = false;
	connection()->getValue("ResetPasswordResult", ret);

	return ret;
}

}
