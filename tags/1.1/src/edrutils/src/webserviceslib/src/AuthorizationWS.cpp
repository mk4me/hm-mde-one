#include <webserviceslib/AuthorizationWS.h>
#include <tinyxml.h>

namespace webservices
{

AuthorizationWS::AuthorizationWS(const WSConnectionPtr & connection)
	: WebServiceT<IAuthorizationWS>(connection)
{
		
}

AuthorizationWS::~AuthorizationWS()
{

}

const bool AuthorizationWS::checkMyLogin() const
{
	WSConnectionPtr c(boost::const_pointer_cast<IWSConnection>(connection()));
	c->setOperation("CheckMyLogin");

	try{
		c->invoke(true);
	}catch(WSConnectionSecurityException & ){
		//nie mam usera
		return false;
	}
	
	//jeżeli problem inny niż prawa do usług
	//rzucam dalej	

	bool ret = false;

	try{
		c->getValue<bool>("CheckMyLoginResult", ret);
	}catch(...){

	}

	return ret;
}

const std::string AuthorizationWS::listMyUserGroupsAssigned() const
{
	WSConnectionPtr c(boost::const_pointer_cast<IWSConnection>(connection()));
	c->setOperation("ListMyUserGroupsAssigned");
	c->invoke();

	return c->xmlResponse();
}

const std::string AuthorizationWS::listUsers() const
{
	WSConnectionPtr c(boost::const_pointer_cast<IWSConnection>(connection()));
	c->setOperation("ListUsers");
	c->invoke();

	return c->xmlResponse();

	/*UserList ret;

	auto r = c->xmlResponse();

	TiXmlDocument document;

	{
		if(!document.Parse(r.c_str())) {
			UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
		}
	}

	TiXmlHandle hDocument(&document);
	TiXmlElement* _element;
	TiXmlHandle hParent(0);

	_element = hDocument.FirstChildElement().Element();
	if(!_element) {
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
	}
	hParent = TiXmlHandle(_element);

	TiXmlElement* trials = hParent.FirstChild("ListUsersResponse").ToElement();
	trials = trials->FirstChildElement("UserList");	
	TiXmlElement* user_details = trials->FirstChildElement("UserDetails");
	while(user_details != nullptr) {
		UserDetails userDetails;

		TiXmlElement* ud = user_details->FirstChildElement();

		if(ud != nullptr){
			userDetails.login = ud->GetText();
			ud->NextSiblingElement();
		}

		if(ud != nullptr){
			userDetails.firstName = ud->GetText();
			ud->NextSiblingElement();
		}

		if(ud != nullptr){
			userDetails.lastName = ud->GetText();			
		}

		ret.push_back(userDetails);

		user_details = user_details->NextSiblingElement();
	}

	return ret;
	*/
}

const std::string AuthorizationWS::getMyUserData() const
{
	WSConnectionPtr c(boost::const_pointer_cast<IWSConnection>(connection()));
	c->setOperation("GetMyUserData");
	c->invoke();

	return c->xmlResponse();
}

void AuthorizationWS::updateUserAccount(const std::string & login, const std::string & email,
	const std::string & pass, const std::string & newPass,
	const std::string & firstName, const std::string & lastName)
{
	connection()->setOperation("UpdateUserAccount");
	connection()->setValue("login", login);
	connection()->setValue("email", email);
	connection()->setValue("pass", pass);
	connection()->setValue("newPass", newPass);
	connection()->setValue("firstName", firstName);
	connection()->setValue("lastName", lastName);
	connection()->invoke();
}

void AuthorizationWS::evokeGroupMembership(const std::string & grantedUserLogin,
	const std::string & groupName)
{
	connection()->setOperation("EvokeGroupMembership");
	connection()->setValue("grantedUserLogin", grantedUserLogin);
	connection()->setValue("groupName", groupName);	
	connection()->invoke();
}

void AuthorizationWS::revokeGroupMembership(const std::string & grantedUserLogin,
	const std::string & groupName)
{
	connection()->setOperation("RevokeGroupMembership");
	connection()->setValue("grantedUserLogin", grantedUserLogin);
	connection()->setValue("groupName", groupName);	
	connection()->invoke();
}

const std::string AuthorizationWS::listSessionPrivileges(const int sessionID) const
{
	WSConnectionPtr c(boost::const_pointer_cast<IWSConnection>(connection()));
	c->setOperation("ListSessionPrivileges");
	c->setValue("sessionID", sessionID);
	c->invoke();
	return c->xmlResponse();

	/*SessionPrivilegeList ret;

	auto r = c->xmlResponse();

	TiXmlDocument document;

	{
		if(!document.Parse(r.c_str())) {
			UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
		}
	}

	TiXmlHandle hDocument(&document);
	TiXmlElement* _element;
	TiXmlHandle hParent(0);

	_element = hDocument.FirstChildElement().Element();
	if(!_element) {
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
	}
	hParent = TiXmlHandle(_element);

	TiXmlElement* trials = hParent.FirstChild("ListSessionPrivilegesResponse").ToElement();
	trials = trials->FirstChildElement("SessionPrivilegeList");	
	TiXmlElement* session_privileges = trials->FirstChildElement("SessionPrivilege");
	while(session_privileges != nullptr) {
		SessionPrivilege sessionPrivilege;

		TiXmlElement* sp = session_privileges->FirstChildElement();

		if(sp != nullptr){
			sessionPrivilege.login = sp->GetText();
			sp->NextSiblingElement();
		}

		if(sp != nullptr){
			sessionPrivilege.canWrite = boost::lexical_cast<bool>(sp->GetText());
		}

		ret.push_back(sessionPrivilege);

		session_privileges = session_privileges->NextSiblingElement();
	}

	return ret;
	*/
}

void AuthorizationWS::grantSessionPrivileges(const std::string & grantedUserLogin,
	const int sessionID, const bool write)
{
	connection()->setOperation("GrantSessionPrivileges");
	connection()->setValue("grantedUserLogin", grantedUserLogin);
	connection()->setValue("sessionID", sessionID);
	connection()->setValue("write", write);
	connection()->invoke();
}

void AuthorizationWS::removeSessionPrivileges(const std::string & grantedUserLogin,
	const int sessionID)
{
	connection()->setOperation("RemoveSessionPrivileges");
	connection()->setValue("grantedUserLogin", grantedUserLogin);
	connection()->setValue("sessionID", sessionID);	
	connection()->invoke();
}

void AuthorizationWS::alterSessionVisibility(const int sessionID, const bool isPublic,
	const bool isWritable)
{
	connection()->setOperation("AlterSessionVisibility");
	connection()->setValue("sessionID", sessionID);
	connection()->setValue("isPublic", isPublic);
	connection()->setValue("isWritable", isWritable);		
	connection()->invoke();
}

}
